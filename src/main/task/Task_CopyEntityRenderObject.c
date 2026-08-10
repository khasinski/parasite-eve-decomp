#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *g_FieldActorListHead __asm__("D_8009D20C");
extern FieldActor *g_PlayerEntity __asm__("D_8009D254");
extern FieldActor *g_CurrentEntity __asm__("D_8009D2F0");
extern u32 D_800B89F8[];

void Render_InitObjectFromTable(RenderObjectEntity *object,
                                RenderObjectEntity *source, int index);
void Render_TransformSkinnedVertices(RenderObjectEntity *object,
                                     u32 *view_matrix);

int Task_CopyEntityRenderObject(int **args) {
    register FieldActor *source asm("$16");
    register int type_id asm("$2");

    type_id = *args[0];
    if (type_id == 0) {
        register FieldActor *player asm("$5");

        player = g_PlayerEntity;
        if (player == 0) {
            return 1;
        }
        source = player;
        goto copy_object;
    }

    source = g_FieldActorListHead;
    if (source == 0) {
        return 1;
    }

    {
        register int wanted_type asm("$5");

        wanted_type = type_id;

search:
        if (source->type_id != wanted_type) {
            goto next;
        }
        if (source->sub_id != *args[1]) {
            goto next;
        }
        if ((source->flags & 0x10) == 0) {
            goto found;
        }

next:
        source = source->next;
        if (source != 0) {
            goto search;
        }
    }

found:
    if (source == 0) {
        return 1;
    }

copy_object:
    Render_InitObjectFromTable(&g_CurrentEntity->render_object,
                               &source->render_object, (s16)*args[2]);
    Render_TransformSkinnedVertices(&g_CurrentEntity->render_object,
                                    D_800B89F8);

    g_CurrentEntity->parent = source;
    g_CurrentEntity->pos_x =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].x << 16;
    g_CurrentEntity->pos_y =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].y << 16;
    g_CurrentEntity->pos_z =
        (s16)g_CurrentEntity->render_object.rotation_overrides[0].z << 16;
    g_CurrentEntity->flags |= 0x2000;

    return 1;
}
