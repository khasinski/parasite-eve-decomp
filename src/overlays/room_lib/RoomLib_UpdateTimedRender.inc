#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"

extern RoomFxSeed8 ROOMLIB_UPDATE_TIMED_RENDER_SEED;
extern char ROOMLIB_UPDATE_TIMED_RENDER_TABLE;
extern RoomSpriteMatrix *D_800BCFA4;
extern int D_800E27EC;

extern void func_800CF3AC(void *table, RoomFxSeed8 *work, int frame);
extern int func_80077CF4(int angle);
extern void func_800D0728(RoomFxTimedRenderState *state, int arg1, int arg2,
                          int arg3, RoomFxSeed8 *arg4, int arg5,
                          int arg6, int arg7, RoomFxSeed8 *work, int arg9,
                          int arg10);

int ROOMLIB_UPDATE_TIMED_RENDER_NAME(int mode, RoomFxTimedRenderState *state) {
    RoomFxSeed8 work;
    RoomFxSeed8 seed;
    RoomFxSeed8 *seedPtr;
    RoomSpriteMatrix **matrixSlot;
    int angle;

    seed = ROOMLIB_UPDATE_TIMED_RENDER_SEED;
    seedPtr = &seed;
    switch (mode) {
    case 1:
        if (state->disabled != 0) {
            goto ret0;
        }
        state->frame++;
        if ((short)state->frame < 24) {
            goto ret0;
        }
        goto ret1;
    case 2:
        if (state->disabled != 0) {
            goto disabled;
        }
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800CF3AC(&ROOMLIB_UPDATE_TIMED_RENDER_TABLE, &work,
                      (short)state->frame);
        angle = func_80077CF4((D_800E27EC << 10) / 24);
        func_800D0728(state, 0x578, 0x640, 0x14, seedPtr, angle, angle, 0,
                      &work, 0x80, 1);
        goto ret0;
    }
ret0:
    return 0;
ret1:
    return 1;
disabled:
    return 0;
}
