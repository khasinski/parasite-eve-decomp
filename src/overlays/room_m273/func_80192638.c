#include "common.h"
#include "pe1/field_actor.h"
#include "room_m273.h"

extern FieldActor *g_PlayerEntity;
extern int FieldEng_VecToAngle(void *, void *);
extern int func_800DFE20(void *, void *);
extern void func_80020C74(void);
extern void func_80192838(RoomSelectionState *);
extern void func_80192D50(char *);

void func_80192638(char *obj) {
    char *slot = *(char **)(obj + 8);

    if (slot[0xE] == 0x10) {
        *(void (**)(void))(obj + 0xC) = (void (*)(void))func_80192664;
    }
}

void func_80192664(RoomSelectionState *typedState) {
    char *stateBytes = (char *)typedState;
    FieldActor *actor = typedState->actor;
    FieldActor *player = g_PlayerEntity;

    if (player->mode < 18) {
        int frame = actor->anim.parts.integer;

        if (frame < 14) {
            return;
        }
        if (frame < 30) {
            void *origin = actor->render_object.model_matrix.translation;
            int mapAngle;

            *(int *)0x1F800008 =
                *(int *)(*(char *volatile *)&actor->render_object.matrices + 0x594) << 16;
            *(int *)0x1F800010 =
                *(int *)(*(char *volatile *)&actor->render_object.matrices + 0x59C) << 16;
            mapAngle = FieldEng_VecToAngle(
                origin, *(char *volatile *)&actor->render_object.matrices + 0x594);

            if (((FieldEng_VecToAngle(
                      origin, g_PlayerEntity->render_object.model_matrix.translation) -
                  mapAngle + 0x40) &
                 0xFFF) < 0x100) {
                if (func_800DFE20(&g_PlayerEntity->pos_x, (void *)0x1F800008) <
                    0x180) {
                    typedState->callback = func_80192838;
                    func_80020C74();
                    {
                        FieldActor *activePlayer = g_PlayerEntity;

                        activePlayer->render_object.animation_source = &actor->render_object;
                        activePlayer->render_object.animation_state = 4;
                        activePlayer->render_object.animation_id = 44;
                        activePlayer->flags |= 0x10000;
                        activePlayer->render_object.flags_9C |= 0x400;
                        activePlayer->render_object.model_matrix = typedState->matrix;
                    }
                    typedState->activated = 1;
                    {
                        FieldActor *positionPlayer = g_PlayerEntity;

                        typedState->savedX = positionPlayer->pos_x;
                        typedState->savedZ = positionPlayer->pos_z;
                    }
                }
            }
            return;
        }
    }

    *((RoomSelectionState *)stateBytes)->signal = 3;
    ((RoomSelectionState *)stateBytes)->callback =
        (void (*)(RoomSelectionState *))func_80192D50;
}
