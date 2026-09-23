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

void func_800C4E50(void *item);

#endif
