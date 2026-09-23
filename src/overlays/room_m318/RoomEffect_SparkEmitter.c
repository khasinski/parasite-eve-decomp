#include "common.h"

typedef struct RoomM318Spark {
    u16 x, y, z, speed;
} RoomM318Spark;

typedef struct RoomM318Emitter {
    char pad[8];
    void *link;
} RoomM318Emitter;

extern void *D_8009D254;
extern u16 D_800942EC, D_800E11EA, D_800E2850[];
extern int D_800E27EC;
extern RoomM318Emitter *D_800F33E0;
extern u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern void func_800CE870(void *, int, void *);
extern int func_800CE560(void *, int, int, void *);
extern RoomM318Spark *func_800CE610(void *);
extern int func_80071A54(void);
extern int func_801939AC(int, RoomM318Spark *);

int func_80193B60(int mode, RoomM318Spark *spark) {
    RoomM318Spark *next;
    register int random asm("$2");
    register int index asm("$3");
    switch (mode) {
    case 0:
        func_800CE870(D_8009D254, 1, spark);
        spark->y = D_800942EC;
        return func_800CE560(D_800F33E0->link, 8, 12, func_801939AC);
    case 1:
        if (D_800E27EC < 40 && (D_800E27EC & 1)) {
            next = func_800CE610(D_800F33E0->link);
            if (next) {
                random = func_80071A54();
                next->x = spark->x + random % 400 - 200;
                next->y = spark->y - 800;
                random = func_80071A54();
                next->z = spark->z + random % 400 - 200;
                next->speed = (func_80071A54() & 7) + 42;
            }
        }
        if (D_800E27EC >= 70) return 1;
        break;
    case 2:
        index = D_800E11EA;
        D_800F3368 = 32;
        D_800F336A = 2;
        D_800F3376 = 32;
        D_800F3378 = 32;
        /* Keep the first four effect settings ahead of the palette lookup. */
        asm("" : : "r"(index) : "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 32;
        D_800F3370 = D_800E2850[index];
        break;
    }
    return 0;
}
