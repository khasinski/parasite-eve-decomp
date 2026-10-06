/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/field_actor.h"
#include "pe1/scene_flags.h"
#include "common.h"
#include "pe1/pm.h"
#include "pe1/task_node.h"

/* The actor list globals this unit owns. */
typedef FieldActor Entity;

Entity *g_FieldActorListHead;
Entity *g_EntityFreeListHead;
Entity *g_PlayerEntity;
u16 g_EntityFreePoolCount;
int g_FieldMoveLock;
Entity *g_CurrentEntity;

void Entity_DispatchCallbacks(FieldActor *a);

void Entity_IntegratePositionFull(FieldActor *a)
{
    int tmp;
    int v0;

    a->base_x = a->pos_x;
    a->base_y = a->pos_y;
    a->base_z = a->pos_z;
    a->saved_rot_x = a->rot_x;
    a->saved_rot_y = a->rot_y;
    a->saved_rot_z = a->rot_z;
    Entity_DispatchCallbacks(a);

    if ((g_FieldMoveLock & 1) == 0) {
        v0 = g_PlayerEntity->state->flags;
        tmp = a->mode;
        if ((v0 & 0xC0) == 0x80) {
            tmp = 0x11;
        }
        Scene_CheckFlagBits(a, D_800943C0, &tmp);
    }

    if (a->flags & 2) {
        a->motion_x += a->gravity_x;
        a->motion_y += a->gravity_y;
        a->motion_z += a->gravity_z;
    }

    a->motion_x += a->accel_x;
    a->motion_y += a->accel_y;
    a->motion_z += a->accel_z;

    a->pos_x += a->motion_x;
    a->pos_y += a->motion_y;
    a->pos_z += a->motion_z;

    a->pos_x += a->delta_x;
    a->pos_y += a->delta_y;
    a->pos_z += a->delta_z;
}

extern int g_GameStateFlags;

void Entity_IntegratePositionConditional(FieldActor *a)
{
    if (g_GameStateFlags & 0x100) {
        Entity_DispatchCallbacks(a);
        return;
    }

    a->base_x = a->pos_x;
    a->base_y = a->pos_y;
    a->base_z = a->pos_z;
    a->saved_rot_x = a->rot_x;
    a->saved_rot_y = a->rot_y;
    a->saved_rot_z = a->rot_z;
    Entity_DispatchCallbacks(a);

    if (a->flags & 2) {
        a->motion_x += a->gravity_x;
        a->motion_y += a->gravity_y;
        a->motion_z += a->gravity_z;
    }

    a->motion_x += a->accel_x;
    a->motion_y += a->accel_y;
    a->motion_z += a->accel_z;

    a->pos_x += a->motion_x;
    a->pos_y += a->motion_y;
    a->pos_z += a->motion_z;

    a->pos_x += a->delta_x;
    a->pos_y += a->delta_y;
    a->pos_z += a->delta_z;
}

/* Actor list maintenance after movement: rolling positions back through
 * the parent chain, following attached parents, freeing dead actors, and
 * running and advancing each actor's three task-node lists. */

extern int g_TaskNodePool[];

void Entity_FreeAllocationBlock(int arg0);
int Util_ReturnTrue(void *unused);
void Task_RunQueue(void);

void Entity_RollbackPositionHierarchy(FieldActor *arg0)
{
    FieldActor *cur;
    FieldActor *child;

    child = arg0->parent;
    if (child != 0) {
        Entity_RollbackPositionHierarchy(child);
        cur = g_FieldActorListHead;
        if (cur != 0) {
            do {
                if (cur->parent == arg0->parent) {
                    cur->pos_x = cur->base_x;
                    cur->pos_y = cur->base_y;
                    cur->pos_z = cur->base_z;
                    cur->field_1a4 = cur->field_1a8;
                }
                cur = cur->next;
            } while (cur != 0);
        }
    } else {
        arg0->pos_x = arg0->base_x;
        arg0->pos_y = arg0->base_y;
        arg0->pos_z = arg0->base_z;
        arg0->field_1a4 = arg0->field_1a8;
        arg0->flags |= 0x40000;
    }
}

void Entity_CopyParentPosition(void) {
    FieldActor *cur = g_FieldActorListHead;

    while (cur != 0) {
        if (cur->parent != 0 && (cur->flags & 0x400000) != 0) {
            cur->pos_x = cur->parent->pos_x;
            cur->pos_y = cur->parent->pos_y;
            cur->pos_z = cur->parent->pos_z;
            cur->rot_x = cur->parent->rot_x;
            cur->rot_y = cur->parent->rot_y;
            cur->rot_z = cur->parent->rot_z;
        }
        cur = cur->next;
    }
}

void Entity_CollectGarbage(void) {
    Entity *cur;
    Entity *next;
    Entity *head_next;
    Entity *free_head;
    Entity *parent;
    int flags;

    cur = g_FieldActorListHead;
    if (cur != 0) {
        do {
            flags = cur->flags;
            parent = cur->parent;
            cur->flags = flags & ~0x800000;

            if (((parent != 0) && (parent->flags & 0x10)) || (flags & 0x10)) {
                Scene_FreeEntityTable(cur);
                next = cur->next;

                if (cur == g_PlayerEntity) {
                    g_PlayerEntity = 0;
                    g_FieldMoveLock &= ~0xD;
                }

                if (cur->allocation_active != 0) {
                    Entity_FreeAllocationBlock(cur->allocation_block);
                    Util_ReturnTrue(&cur->render_object);
                }

                if (cur->prev == 0) {
                    head_next = cur->next;
                    free_head = g_EntityFreeListHead;
                    g_EntityFreeListHead = cur;
                    g_FieldActorListHead = head_next;
                    head_next->prev = 0;
                    cur->next = free_head;
                } else {
                    cur->prev->next = cur->next;
                    if (cur->next != 0) {
                        cur->next->prev = cur->prev;
                    }
                    cur->next = g_EntityFreeListHead;
                    g_EntityFreeListHead = cur;
                }

                g_EntityFreePoolCount--;
                cur = next;
            } else {
                cur = cur->next;
            }
        } while (cur != 0);
    }
}

void Entity_DispatchCallbacks(Entity *arg0) {
    int i;
    int *ptr;
    int callback;

    i = 0;
    ptr = (int *)arg0->task_node_lists;
    g_CurrentEntity = arg0;
    do {
        callback = *ptr;
        g_TaskNodePool[0] = callback;
        if (callback != 0) {
            Task_RunQueue();
        }
        i++;
        ptr++;
    } while ((unsigned int)i < 3U);
}

void Entity_TickAnimSequences(FieldActor *arg0) {
    unsigned int i;
    TaskNode *node;

    i = 0;
    do {
        node = (TaskNode *)arg0->task_node_lists[0];
        if (node != 0) {
            do {
                if (node->next_value != 0) {
                    node->current = node->next_value;
                    node->active = 1;
                    node->flags &= ~0x60;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
        arg0 = (FieldActor *)((char *)arg0 + 4);
    } while (i < 3U);
}
