#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM123Burst {
    u16 frame;
    u16 phase;
    u16 x, y, z;
    u16 pad;
} RoomM123Burst;

typedef struct RoomM123Wave {
    u16 x, y, z, pad;
    s16 state;
    u16 frame;
} RoomM123Wave;

typedef struct RoomM123Pool {
    char pad[8];
    void *pool;
} RoomM123Pool;

extern GteShortVector D_8018F1CC;
extern RoomM123Pool *D_800F32D0, *D_800F33E0;
extern int D_800E27EC;
extern volatile u16 D_800E11E8;
extern u16 D_800E2850[];
extern volatile u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern volatile u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern int func_80194F68(int, RoomM123Wave *);
extern void func_800CE8F0(void *, int, void *, void *);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern int func_800CE560(void *, int, int, int (*)(int, RoomM123Wave *));
extern RoomM123Wave *func_800CE610(void *);
extern int func_80071A54(void);

int func_80195114(int mode, RoomM123Burst *burst, int *choice) {
    GteShortVector position = D_8018F1CC;
    RoomM123Wave *child;
    int value;

    switch (mode) {
    case 0:
        value = *choice ? 34 : 40;
        func_800CE8F0(D_800F32D0->pool, value, &position, &burst->x);
        burst->frame = 0;
        burst->phase = 0;
        func_800D3F64(0x581, func_800D3FD8());
        return func_800CE560(D_800F33E0->pool, 12, 24, func_80194F68);
    case 1:
        if (D_800E27EC < 32) {
            child = func_800CE610(D_800F33E0->pool);
            if (child != 0) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                {
                    int sample = func_80071A54();
                    int previous = child->x;
                    child->x = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->y;
                    child->y = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->z;
                    child->z = (previous - 128) + (sample & 255);
                }
                child->state = 0;
                child->frame = 0;
            }
        }
        if (D_800E27EC >= 8) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11E8;
        int palette;
        D_800F3368 = 16;
        D_800F336A = 1;
        D_800F3376 = 16;
        D_800F3378 = 16;
        palette = D_800E2850[paletteIndex];
        /* Keep the palette lookup ahead of the remaining register writes. */
        asm volatile("" ::: "memory");
        D_800F336C = 2;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 0;
        D_800F3370 = palette;
        break;
    }
    }
    return 0;
}
