/* Arc sprite callback, its pool controller, and adjacent seed helper. */
#include "common.h"
#include "room_m273_arc.h"
#include "pe1/scene_fx.h"

extern s32 D_800E27EC;
extern s32 D_800966EC[];
extern char D_8019AB70[];
extern char D_8019AD7C[];

void func_800D004C(RoomM273ArcCallbackState *state, s32 scale0, s32 scale1,
                   s32 count, volatile s16 *rotation, s32 offset0,
                   s32 offset1, void *resource0, void *resource1, s32 angle,
                   s32 one);

#include "common.h"

#include "room_m273_effects.h"
extern s32 D_800E27EC;
extern u8 D_8019AF6B;
extern u16 D_8019AEFC;
extern u16 D_8019AEFE;
extern u16 D_8019AF00;

extern s32 func_8019A720(s32 mode, RoomM273ArcCallbackState *state);
extern s32 func_800CE560(void *arg0, s32 arg1, s32 arg2, int (*arg3)());
extern char *func_800CE610(void *arg0);

s32 func_8019A628(s32 arg0) {
    char *obj;

    if (arg0 != 1) {
        if (arg0 >= 2) {
            return 0;
        }
        if (arg0 != 0) {
            return 0;
        }
        return func_800CE560(D_800F33E0->pool, 8, 5, func_8019A720);
    }

    if (D_8019AF6B != 0) {
        return 2;
    }
    if ((D_800E27EC & 3) != 0) {
        return 0;
    }

    obj = func_800CE610(D_800F33E0->pool);
    if (obj != 0) {
        *(u16 *)(obj + 0) = D_8019AEFC;
        *(u16 *)(obj + 2) = D_8019AEFE;
        *(u16 *)(obj + 4) = D_8019AF00;
        *(u16 *)(obj + 6) = 0;
    }

    return 0;
}

extern short D_8019AF9A;

short *func_8019A70C(void *arg0, short arg1) {
    short *ptr = &D_8019AF9A;

    *ptr = arg1;
    return ptr - 0x17;
}

s32 func_8019A720(s32 mode, RoomM273ArcCallbackState *state) {
    register RoomM273ArcCallbackState *obj asm("$5") = state;
    s32 day;
    s32 rotationValue;
    s32 packed;
    s32 result;
    RoomM273ArcCallbackState *callState;
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
