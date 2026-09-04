#include "common.h"
#include "pe1/scene_fx.h"

typedef struct RoomArcCallbackState {
    u16 unused0;
    u16 value;
    u16 unused4;
    u16 step;
} RoomArcCallbackState;

extern s32 D_800E27EC;
extern s32 D_800966EC[];
extern char D_8019AB70[];
extern char D_8019AD7C[];

void func_800D004C(RoomArcCallbackState *state, s32 scale0, s32 scale1,
                   s32 count, volatile s16 *rotation, s32 offset0,
                   s32 offset1, void *resource0, void *resource1, s32 angle,
                   s32 one);

s32 func_8019A720(s32 mode, RoomArcCallbackState *state) {
    register RoomArcCallbackState *obj asm("$5") = state;
    s32 day;
    s32 rotationValue;
    s32 packed;
    s32 result;
    RoomArcCallbackState *callState;
    s32 count;
    volatile s16 rotationX;
    volatile s16 rotationY;
    volatile s16 rotationZ;
    volatile s16 rotationPad;

    if (mode == 1) {
        if (D_800E27EC >= 0x10) {
            result = 1;
            goto epilogue;
        }
        obj->value -= obj->step;
        obj->step++;
        goto return_zero;
    }
    if (mode != 2) {
        result = 0;
        goto epilogue;
    }
    {
        rotationX = 0;
        rotationY = 0;
        rotationPad = 0;
        callState = obj;
        day = D_800E27EC - 1;
        count = 0x10;
        rotationValue = day << 7;
        rotationZ = rotationValue;
        packed = D_800966EC[(day << 6) & 0xFC0];
        func_800D004C(callState, 0x100, 0x100, count, &rotationX,
                      ((packed << 16) >> 16) + 0x400,
                      ((packed << 16) >> 16) + 0x400,
                      D_8019AB70, D_8019AD7C, packed >> 21, 1);
    }

return_zero:
    result = 0;

epilogue:
    return result;
}
