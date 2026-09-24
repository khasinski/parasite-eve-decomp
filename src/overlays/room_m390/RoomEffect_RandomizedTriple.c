#include "common.h"

typedef struct RoomTripleVector {
    u16 x, y, z, pad;
} RoomTripleVector;

typedef struct RoomTripleContext {
    u8 pad[8];
    void *pool;
} RoomTripleContext;

typedef struct RoomTriplePoolOwner {
    u8 pad[0x238];
    u8 *map;
} RoomTriplePoolOwner;

typedef struct RoomTripleParticle {
    u16 x, y, z, pad;
    s16 motion[4];
    u16 kind, phase;
} RoomTripleParticle;

extern RoomTripleVector D_8018F1DC;
extern RoomTripleContext *D_800F32D0, *D_800F33E0;
extern u8 *D_800E2368;
extern int D_800E27EC;
extern u16 D_800E11EA, D_800E2850[];
extern u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern int func_80193300(int, RoomTripleParticle *);
extern int func_800CE560(void *, int, int, int (*)(int, RoomTripleParticle *));
extern RoomTripleParticle *func_800CE610(void *);
extern void func_800CE8F0(void *, int, RoomTripleVector *, RoomTripleVector *);
extern int func_80071A54(void);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern void func_800CEAE8(void *, s16 *, s16 *);

int func_80193890(int mode)
{
    RoomTripleVector position = D_8018F1DC;
    RoomTripleVector output;
    s16 motion[4];
    RoomTripleParticle *particle;
    int kind;

    switch (mode) {
    case 0:
        if (D_800E2368[13] != 0) {
            void *object = D_800F32D0->pool;
            if (object != 0) {
                void *inner = *(void **)object;
                if (inner != 0) {
                    u8 *status = *(u8 **)((u8 *)inner + 0x18);
                    if (*status == 1) *status = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->pool, 20, 32, func_80193300);
    case 1:
        func_800CE8F0(D_800F32D0->pool, 7, &position, &output);
        if (D_800E27EC == 8)
            func_800D3F64(0x593, func_800D3FD8());
        if (D_800E27EC < 74) {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->x = output.x;
                particle->y = output.y;
                particle->z = output.z;
                motion[0] = (func_80071A54() & 31) - 16;
                motion[1] = (func_80071A54() & 31) - 16;
                motion[2] = -32 - (func_80071A54() & 31);
                kind = func_80071A54() % 3;
                if (kind == mode) {
                    motion[0] = motion[0] * 3 / 2;
                    motion[1] = motion[1] * 3 / 2;
                    motion[2] = motion[2] * 3 / 2;
                }
                func_800CEAE8(((RoomTriplePoolOwner *)D_800F32D0->pool)->map + 0xE0,
                              motion, particle->motion);
                particle->kind = kind;
                particle->phase = 0;
            }
        }
        if (D_800E27EC < 2) goto ret0;
        return 2;
    case 2: {
        u16 palette;
        D_800F3368 = 32;
        D_800F336A = 2;
        D_800F3376 = 32;
        D_800F3378 = 32;
        palette = D_800E2850[D_800E11EA];
        /* Keep the palette lookup ahead of the remaining effect-register writes. */
        asm volatile("" : : "r"(palette) : "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 8;
        D_800F3370 = palette;
        break;
    }
    }
ret0:
    return 0;
}
