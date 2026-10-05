#include "common.h"
#include "pe1/entity_frame_update.h"
#include "pe1/task_node.h"

typedef RenderAnimationLookupEntry HitSphere;


void Entity_RollbackPositionHierarchy(FieldActor *actor);

/* Start SELF's contact task for OTHER unless an active one already exists. */
static inline void Scene_LinkContactTask(FieldActor *self, FieldActor *other)
{
    TaskNode *node;
    TaskNode *head;
    int found;

    if (self->script_cursor_1a0 != 0) {
        found = 0;
        node = (TaskNode *)self->task_node_lists[1];
        while (!found) {
            if (node == 0) {
                break;
            }
            if ((node->flags & 1) && !(node->flags & 0x10) &&
                node->trigger_value == other->field_sfx_id) {
                found = 1;
            }
            node = node->next;
        }
        if (!found) {
            node = Task_AllocNode(self->script_cursor_1a0, 0);
            node->flags |= 1;
            node->trigger_value = other->field_sfx_id;
            node->target18.coordinate = other->type_id;
            node->target1c = other->sub_id;
            head = (TaskNode *)self->task_node_lists[1];
            if (head != 0) {
                node->next = head;
                head->prev = node;
            }
            self->task_node_lists[1] = (struct FieldActorNode *)node;
        }
    }
}

/*
 * Per-frame actor contact pass. Clears the contact flags, then tests every
 * pair of active actors: body spheres first, then (for actors with an
 * allocation block) the x/z cylinders. Cylinder contact rolls back actors
 * moving into each other, restores their saved yaw and flags player contact;
 * touching or near-missing pairs start each other's contact task. In battle,
 * or when the cylinders are absent, the per-part hit spheres are tested and
 * the contact callbacks run (typed actors only flag 0x40000 instead).
 */
void Scene_UpdateEntityPositions(void)
{
    FieldActor *self;
    FieldActor *other;
    HitSphere *body;
    HitSphere *cylinder;
    HitSphere *selfPart;
    HitSphere *otherPart;
    int selfSide;
    int selfTop;
    int otherRadius;
    int selfRadius;
    int distance;
    int reach;
    int bx, by, bz; /* body sphere deltas, squared in place */
    int cx, cz;     /* cylinder deltas, squared in place */
    int px, py, pz; /* part sphere deltas, squared in place */
    unsigned int i;
    unsigned int j;
    int heading[2];
    int offset[2];
    int dot;

    for (self = g_FieldActorListHead; self != 0; self = self->next) {
        self->flags &= ~0x02040000;
    }

    for (self = g_FieldActorListHead; self != 0; self = self->next) {
        if (self->flags & 0x20) {
            continue;
        }
        body = &self->render_object.hit_body;
        cylinder = &self->render_object.hit_cylinder;
        selfSide = cylinder->radius * self->move_speed / 4096;
        selfTop = body->radius * self->move_speed / 4096;
        for (other = self->next; other != 0; other = other->next) {
            if (self->parent == other || other->parent == self) {
                continue;
            }
            if ((self->flags & 0x20000) && other != g_PlayerEntity) {
                continue;
            }
            if ((other->flags & 0x20000) && self != g_PlayerEntity) {
                continue;
            }
            if (other->flags & 0x20) {
                continue;
            }
            otherRadius = other->render_object.hit_body.radius * other->move_speed / 4096;
            bx = body->value0 - other->render_object.hit_body.value0;
            bx *= bx;
            by = body->value1 - other->render_object.hit_body.value1;
            by *= by;
            bz = body->value2 - other->render_object.hit_body.value2;
            bz *= bz;
            reach = otherRadius + selfTop;
            reach = reach * reach;
            distance = bx + by + bz;
            if (self->allocation_active == 0 || other->allocation_active == 0) {
                if (distance <= reach) {
                link:
                    Scene_LinkContactTask(self, other);
                    Scene_LinkContactTask(other, self);
                }
                continue;
            }
            if (distance > reach) {
                continue;
            }
            if (cylinder->radius != 0 && other->render_object.hit_cylinder.radius != 0) {
                otherRadius = other->render_object.hit_cylinder.radius * other->move_speed / 4096;
                cx = cylinder->value0 - other->render_object.hit_cylinder.value0;
                cx *= cx;
                cz = cylinder->value2 - other->render_object.hit_cylinder.value2;
                cz *= cz;
                reach = otherRadius + selfSide;
                reach = reach * reach;
                distance = cx + cz;
                if (distance <= reach) {
                    heading[0] = (self->pos_x - self->base_x) >> 16;
                    heading[1] = (self->pos_z - self->base_z) >> 16;
                    offset[0] = cylinder->value0 - other->render_object.hit_cylinder.value0;
                    offset[1] = cylinder->value2 - other->render_object.hit_cylinder.value2;
                    dot = heading[0] * offset[0];
                    dot += heading[1] * offset[1];
                    if (dot < 0) {
                        Entity_RollbackPositionHierarchy(self);
                    }
                    heading[0] = (other->pos_x - other->base_x) >> 16;
                    heading[1] = (other->pos_z - other->base_z) >> 16;
                    dot = heading[0] * offset[0];
                    dot += heading[1] * offset[1];
                    if (dot > 0) {
                        Entity_RollbackPositionHierarchy(other);
                    }
                    if (self != g_PlayerEntity && !(self->flags & 0x01000000)) {
                        self->rot_y = self->saved_rot_y;
                    }
                    if (other != g_PlayerEntity && !(other->flags & 0x01000000)) {
                        other->rot_y = other->saved_rot_y;
                    }
                    if (self == g_PlayerEntity) {
                        other->flags |= 0x02000000;
                    } else if (other == g_PlayerEntity) {
                        self->flags |= 0x02000000;
                    }
                    Scene_LinkContactTask(self, other);
                    Scene_LinkContactTask(other, self);
                    if (!(g_GameStateBlock.flags & 2)) {
                        continue;
                    }
                } else if (!(g_GameStateBlock.flags & 2) && distance <= reach + 10000) {
                    /* Recorded crutch debt: retail's near miss jumps back into
                     * the contact-task block of the no-allocation case; no
                     * structured form places that block first. */
                    goto link;
                }
            }
            if (self->type_id != 0 && other->type_id != 0) {
                self->flags |= 0x40000;
                other->flags |= 0x40000;
                continue;
            }
            selfPart = self->render_object.animation_entries;
            for (i = 0; i < self->render_object.header->animation_entry_count; i++, selfPart++) {
                selfRadius = selfPart->radius * self->move_speed / 4096;
                otherPart = other->render_object.animation_entries;
                for (j = 0; j < other->render_object.header->animation_entry_count; j++, otherPart++) {
                    otherRadius = otherPart->radius * other->move_speed / 4096;
                    px = selfPart->value0 - otherPart->value0;
                    px *= px;
                    py = selfPart->value1 - otherPart->value1;
                    py *= py;
                    pz = selfPart->value2 - otherPart->value2;
                    pz *= pz;
                    reach = otherRadius + selfRadius;
                    reach = reach * reach;
                    distance = px + py + pz;
                    if (distance <= reach) {
                        if (self->contact_callback != 0) {
                            self->contact_callback(self, selfPart->animation_id, other,
                                                   otherPart->animation_id);
                        }
                        if (other->contact_callback != 0) {
                            other->contact_callback(other, otherPart->animation_id, self,
                                                    selfPart->animation_id);
                        }
                    }
                }
            }
        }
    }
}
