#ifndef ROOM_M089_H
#define ROOM_M089_H

#include "../room_lib/room_lib.h"

extern short D_800966EC[][2];
extern volatile unsigned short D_800E11FA;
extern unsigned short D_800E2850[];
extern int D_800E27EC;
extern volatile short D_800F3368;
extern volatile short D_800F336A;
extern volatile short D_800F336C;
extern volatile short D_800F336E;
extern volatile short D_800F3370;
extern volatile short D_800F3376;
extern volatile short D_800F3378;
extern void *D_800B0E64;
extern FieldActorNode *D_8009D20C;

extern int func_80071A54(void);
extern void func_8006DF50(void *channel, int id, int value, int volume,
                          int pan);
extern void func_801924F8(void);

#define ROOM_M089_LOAD_EFFECT_LOOKUP(out, index) \
    asm volatile("sll %1,%1,1\n\t" \
                 "lui $1,%%hi(D_800E2850)\n\t" \
                 "addu $1,$1,%1\n\t" \
                 "lhu %0,%%lo(D_800E2850)($1)" \
                 : "=r"(out), "=r"(index) : "1"(index) : "$1")

#endif
