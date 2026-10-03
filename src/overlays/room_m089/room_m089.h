#ifndef ROOM_M089_H
#define ROOM_M089_H

#include "../room_lib/room_lib.h"

extern short D_800966EC[][2];
extern unsigned short D_800E11FA;
extern unsigned short D_800E2850[];
extern int D_800E27EC;

/* The sprite parameter block at 0x800F3368 as one record. */
typedef struct RoomM089EffectParameters {
    unsigned short parameter00;
    unsigned short parameter02;
    unsigned short palette;
    unsigned short parameter06;
    unsigned short tpage;
    unsigned short parameter0A;
    short depth;
    unsigned short extent_x;
    unsigned short extent_y;
} RoomM089EffectParameters;

extern RoomM089EffectParameters D_800F3368;
extern void *D_800B0E64;
extern FieldActorNode *D_8009D20C;

extern int func_80071A54(void);
extern void func_8006DF50(void *channel, int id, int value, int volume,
                          int pan);
extern void func_801924F8(void);

#endif
