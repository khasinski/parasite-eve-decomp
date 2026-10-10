#include "common.h"
#include "pe1/field_actor.h"

extern FieldActor *D_8009D20C;
extern FieldActor *D_8009D254;
extern FieldActor *D_8009D2F0;
extern u32 D_800B89F8[];

void Render_InitObjectFromTable(RenderObjectEntity *object,
                                RenderObjectEntity *source, int index);

int Task_CopyEntityRenderObject(int **args) {
    FieldActor *source;
    int type_id;

    type_id = *args[0];
    if (type_id == 0) {
        FieldActor *player;

        player = D_8009D254;
        if (player == 0) {
            return 1;
        }
        source = player;
    } else {
        source = D_8009D20C;
        if (source == 0) {
            return 1;
        }

        {
            int wanted_type;

            wanted_type = type_id;

            do {
                if (source->type_id != wanted_type) {
                    source = source->next;
                } else if (source->sub_id != *args[1]) {
                    source = source->next;
                } else if ((source->flags & 0x10) == 0) {
                    break;
                } else {
                    source = source->next;
                }
            } while (source != 0);
        }

        if (source == 0) {
            return 1;
        }
    }

    Render_InitObjectFromTable(&D_8009D2F0->render_object,
                               &source->render_object, (s16)*args[2]);
    Render_TransformSkinnedVertices(&D_8009D2F0->render_object,
                                    D_800B89F8);

    D_8009D2F0->parent = source;
    D_8009D2F0->pos_x =
        (s16)D_8009D2F0->render_object.rotation_overrides[0].x << 16;
    D_8009D2F0->pos_y =
        (s16)D_8009D2F0->render_object.rotation_overrides[0].y << 16;
    D_8009D2F0->pos_z =
        (s16)D_8009D2F0->render_object.rotation_overrides[0].z << 16;
    D_8009D2F0->flags |= 0x2000;

    return 1;
}
