#include "room_m273_effects.h"

typedef RoomM273PointerPoolEffect Effect;
typedef struct { unsigned char unknown[58]; unsigned short value; } Object;
typedef struct { int unknown[2]; Object *object; } Context;
typedef struct { short zero; unsigned short value,phase,one; } Parameters;
extern int D_800E27EC,D_800966EC[];
extern Context *D_800F32D0;
extern unsigned char D_8019ACDC[],D_8019ACE0[];
extern void func_800D0728(void *,int,int,int,Parameters *,int,int,void *,void *,int,int);

int func_8019A4CC(int mode,Effect *effect) {
    Parameters parameters;
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int stack_pad[2];
    if(mode==1) {
        if(D_800E27EC>=16) return 1;
    } else if(mode==2) {
        register int frame asm("$4")=D_800E27EC-1;
        unsigned int sizeOffset=((unsigned int)frame<<8)&0x3F00;
        unsigned int shadeOffset=((unsigned int)frame<<9)&0x3E00;
        int size=*(short *)((char *)D_800966EC+sizeOffset)*2+4096;
        int shade=(short)*(int *)((char *)D_800966EC+shadeOffset)>>5;
        parameters.zero=0;
        parameters.value=D_800F32D0->object->value;
        parameters.phase=(unsigned int)frame<<8;
        parameters.one=1;
        func_800D0728(effect->position,32,96,8,&parameters,size,size,
            D_8019ACDC,D_8019ACE0,shade,1);
    }
    return 0;
}

/* Arc spawn-state helper, callback, and pool controller. */
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

extern short D_8019AF56;
extern short D_8019AF58;
extern short D_8019AF5A;
extern short D_8019AF5C;
extern short D_8019AF64;
extern u16 D_8019AEFC;

void *func_8019A5D4(int unused, int value, int flag) {
    D_8019AF56 = value;
    D_8019AF58 = value;
    value++;
    D_8019AF5C = value * 6;
    D_8019AF5A = flag;
    D_8019AF64 = flag != 0 ? 0x52 : 0xE;

    return &D_8019AEFC;
}

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
