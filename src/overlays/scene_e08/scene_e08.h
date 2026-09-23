#ifndef SCENE_E08_H
#define SCENE_E08_H

#include "pe1/room_fx.h"

typedef struct {
    RoomFxEmitterParams emitters[2];
    char sourceData[2][0x100];
    s16 field_230[2];
    char pad_234[0x20];
    s16 field_254[4];
    s16 field_25C[4];
    s16 field_264;
} Ovl178FadeState;

typedef struct {
    char pad_00[0x18];
    s32 x;
    s32 y;
    s32 z;
} Ovl178Position;

typedef struct {
    s32 field_00;
    s32 field_04;
    s32 field_08;
    char pad_0C[4];
    s32 field_10;
    s32 field_14;
    s32 field_18;
    char pad_1C[4];
    s16 field_20;
    s16 field_22;
    s16 field_24;
    char pad_26[2];
    u8 field_28;
    char pad_29;
    s16 field_2A;
    s32 field_2C;
} Ovl178EffectState;

Ovl178Position *func_800C2B50(void);
s32 *func_800C2B10(int index);
s32 *func_800C2B28(int index);
void func_800C4E50(void *item);

#endif
