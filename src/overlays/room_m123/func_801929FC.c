#include "common.h"

typedef struct RoomM123Vector {
    u16 x, y, z;
} RoomM123Vector;

typedef struct RoomM123Pool {
    char pad[8];
    void *pool;
} RoomM123Pool;

extern RoomM123Pool *D_800F33E0;
extern int D_800E27EC;
extern volatile u16 D_800E11EA;
extern u16 D_800E2850[];
extern volatile u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern volatile u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern int func_8019251C(void);
extern int func_800CE560(void *, int, int, int (*)(void));
extern RoomM123Vector *func_800CE610(void *);

int func_801929FC(int mode) {
    int frame, index;
    RoomM123Vector *position;

    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 16, func_8019251C);
    case 1:
        frame = D_800E27EC;
        if (frame == (frame / 6) * 6) {
            if (frame >= 40) return 2;
            for (index = 0; index < 3; index++) {
                position = func_800CE610(D_800F33E0->pool);
                if (position != 0) {
                    position->x = 1;
                    position->y = 0;
                    position->z = index << 12;
                }
            }
            if (D_800E27EC < 25) {
                position = func_800CE610(D_800F33E0->pool);
                if (position != 0) {
                    position->x = 0;
                    position->y = 0;
                    position->z = 0;
                }
            }
        }
        if (D_800E27EC >= 40) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11EA;
        int palette;
        D_800F3368 = 16;
        D_800F336A = 1;
        D_800F3376 = 16;
        D_800F3378 = 16;
        palette = D_800E2850[paletteIndex];
        asm volatile("" ::: "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 12;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
