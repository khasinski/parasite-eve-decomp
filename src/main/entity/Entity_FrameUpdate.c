/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 --use-comm-section */
#include "common.h"
#include "pe1/entity_frame_update.h"
#include "pe1/gte.h"
#include "pe1/scene_flags.h"
#include "pe1/pm.h"
#include "pe1/task_node.h"

/* Per-frame entity driver: callbacks, menu and battle hooks, camera, model
 * matrices, transform and draw, then animation and position updates. */
void Entity_FrameUpdate(void) {
    FieldActor *entity;
    FieldActor *focus;
    int camera[3];
    GteMatrix scale;

    if (!(g_GameStateBlock.flags & 4)) {
        for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
            if (entity->frame_callback != 0) {
                entity->frame_callback(entity);
            }
        }
    }

    if (!(D_800B0CD8[0] & 0x200)) {
        if (g_GameStateBlock.flags & 2) {
            Battle_Update();
        } else {
            if (g_PlayerEntity != 0 && g_PlayerEntity->state != 0 && (D_8009D1F4[0] & 0x80) &&
                !(g_GameStateBlock.flags & 0x2000) && !(D_800B0CD8[0] & 0x3400)) {
                Inventory_OpenAyaItemList(0);
                Render_BeginSceneLoad();
                g_GameStateBlock.flags |= 4;
                D_800B0CD8[0] |= 0x9000;
            }
            D_8009D2A4[0] = Menu_RunFrameWithArg(D_800A76D8);
            if (D_8009D2A4[0] != 0) {
                Render_BeginSceneLoad();
                g_GameStateBlock.flags &= ~4;
                D_800B0CD8[0] &= ~0x9000;
            }
        }

        func_80069660();
        if (!(g_GameStateBlock.flags & 4)) {
            func_8001A9F8();
        }
        Entity_CopyParentPosition();
        Scene_IsNotBattleMode();

        focus = g_PlayerEntity;
        if (focus != 0) {
            camera[0] = focus->pos_x;
            camera[1] = focus->pos_y - (D_800BCFFE[0] << 16);
            camera[2] = focus->pos_z;
        } else {
            camera[2] = 0;
            camera[1] = 0;
            camera[0] = 0;
            for (focus = g_FieldActorListHead; focus != 0; focus = focus->next) {
                if (focus->model_state.allocation_active != 0) {
                    camera[0] = focus->pos_x;
                    camera[1] = focus->pos_y - (D_800BCFFE[0] << 16);
                    camera[2] = focus->pos_z;
                    break;
                }
            }
        }
        Render_SetViewport(camera);

        if (!(g_GameStateBlock.flags & 4)) {
            Render_SetGteScreenOffset();
            for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
                if (!(entity->flags & 0x10040)) {
                    GteMatrix *matrix = (GteMatrix *)&entity->render_object.model_matrix;

                    matrix->t[0] = (s16)(entity->pos_x >> 16);
                    matrix->t[1] = (s16)(entity->pos_y >> 16);
                    matrix->t[2] = (s16)(entity->pos_z >> 16);
                    entity->render_object.table_value2c = entity->rot_x;
                    entity->render_object.table_value2e = entity->rot_y;
                    entity->render_object.table_value30 = entity->rot_z;
                    RotMatrix((GteShortVector *)&entity->render_object.table_value2c, matrix);
                    scale.m[0][0] = entity->move_speed;
                    scale.m[1][1] = entity->move_speed;
                    scale.m[2][2] = entity->move_speed;
                    scale.t[2] = 0;
                    scale.t[1] = 0;
                    scale.t[0] = 0;
                    scale.m[2][1] = 0;
                    scale.m[2][0] = 0;
                    scale.m[1][2] = 0;
                    scale.m[1][0] = 0;
                    scale.m[0][2] = 0;
                    scale.m[0][1] = 0;
                    gte_CompMatrix(matrix, &scale, matrix);
                }
            }
        }

        Render_SetGteScreenOffset();
        for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
            RenderObjectEntity *object;

            if (entity == g_PlayerEntity && (D_800B0CD8[0] & 0x40000)) {
                continue;
            }
            if (entity->model_state.allocation_active != 0) {
                entity->render_object.header->scale = entity->move_speed;
            }
            if (entity->flags & 0x40) {
                if (entity->flags & 0x2000) {
                    Render_TransformSkinnedVertices(&entity->render_object, D_800B89F8);
                    entity->pos_x = (s16)entity->render_object.rotation_overrides[0].x << 16;
                    entity->pos_y = (s16)entity->render_object.rotation_overrides[0].y << 16;
                    entity->pos_z = (s16)entity->render_object.rotation_overrides[0].z << 16;
                } else if (entity->model_state.allocation_active == 0) {
                    entity->render_object.hit_body.value0 = entity->pos_x >> 16;
                    entity->render_object.hit_body.value1 = entity->pos_y >> 16;
                    entity->render_object.hit_body.value2 = entity->pos_z >> 16;
                }
            } else {
                object = &entity->render_object;
                Render_InitRoomPrimState(object);
                if (entity->action_data != 0) {
                    Anim_BuildRotationMatrices(object, entity->action_data, (s16)entity->anim.parts.integer, 1);
                }
                Render_TransformVertices(object);
                Render_TransformSkinnedVertices(object, D_800B89F8);
                if (!(entity->flags & 0x20000000)) {
                    Render_TransformMorphVertices(object, D_800B89F8);
                    Render_DrawEntity(object, D_800B89F8);
                    if (entity->flags & 0x8800) {
                        entity->render_object.flags_9C |= 1;
                    }
                    Render_DrawRoom(entity);
                }
            }
        }

        Scene_LoadEntityTextures();
        func_80069594();
        Render_ResetGteScreenOffset();

        if (!(g_GameStateBlock.flags & 4)) {
            if (g_GameStateBlock.flags & 0x100) {
                if (!(D_800B0CD8[0] & 0x40000) && g_PlayerEntity != 0) {
                    Entity_AdvanceAnim((BattleEntity *)g_PlayerEntity);
                }
            } else {
                for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
                    if ((entity != g_PlayerEntity || !(D_800B0CD8[0] & 0x40000)) &&
                        !(entity->flags & 0x800040)) {
                        Entity_AdvanceAnim((BattleEntity *)entity);
                    }
                }
            }
        }
    }

    if (!(g_GameStateBlock.flags & 4)) {
        Scene_UpdateEntityPositions();
        func_80012774();
        Entity_CollectGarbage();
        for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
            entity->flags &= ~0x10000000;
        }
    }
}

/* Per-frame actor list passes: position integration, rollback, parent
 * following, garbage collection, callbacks and animation sequences. */
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


void Entity_IntegratePositionConditional(FieldActor *a)
{
    if (g_GameStateBlock.flags & 0x100) {
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

                if (cur->model_state.allocation_active != 0) {
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
                    node->current.value = node->next_value;
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
