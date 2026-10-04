/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G4 */
#include "common.h"
#include "pe1/entity_frame_update.h"
#include "pe1/gte.h"

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
                if (focus->allocation_active != 0) {
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
            if (entity->allocation_active != 0) {
                entity->render_object.header->scale = entity->move_speed;
            }
            if (entity->flags & 0x40) {
                if (entity->flags & 0x2000) {
                    Render_TransformSkinnedVertices(&entity->render_object, D_800B89F8);
                    entity->pos_x = (s16)entity->render_object.rotation_overrides[0].x << 16;
                    entity->pos_y = (s16)entity->render_object.rotation_overrides[0].y << 16;
                    entity->pos_z = (s16)entity->render_object.rotation_overrides[0].z << 16;
                } else if (entity->allocation_active == 0) {
                    entity->render_object.animation_value74 = entity->pos_x >> 16;
                    entity->render_object.animation_value76 = entity->pos_y >> 16;
                    entity->render_object.animation_value78 = entity->pos_z >> 16;
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
                    Entity_AdvanceAnim(g_PlayerEntity);
                }
            } else {
                for (entity = g_FieldActorListHead; entity != 0; entity = entity->next) {
                    if ((entity != g_PlayerEntity || !(D_800B0CD8[0] & 0x40000)) &&
                        !(entity->flags & 0x800040)) {
                        Entity_AdvanceAnim(entity);
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
