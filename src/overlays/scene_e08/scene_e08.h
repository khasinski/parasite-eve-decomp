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

typedef struct {
    u8 pad_00[8];
    s16 field_08;
    s16 field_0A;
    s16 offsets[12];
    union {
        s16 variation[3];
        u8 flags[10];
    } tail;
} Ovl178RandomizedEffect;

Ovl178Position *func_800C2B50(void);
s32 *func_800C2B10(int index);
s32 *func_800C2B28(int index);
extern s16 D_80199590[];
extern s16 D_80199658[];
extern s32 D_8019956C;
extern s32 D_8019957C;
void func_800C4E50(void *item);

#endif
