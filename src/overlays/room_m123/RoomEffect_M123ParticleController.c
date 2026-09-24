#include "common.h"
#include "pe1/gte_types.h"

typedef struct RoomM123Particle {
    u16 frame;
    u16 offset;
    s16 scale;
} RoomM123Particle;

typedef struct RoomM123Context {
    u8 pad[8];
    void *pool;
} RoomM123Context;

extern GteShortVector D_8018F1E0;
extern RoomM123Context *D_800F32D0, *D_800F33E0;
extern int D_800E27EC;
extern s16 D_801956A0, D_801956A4;
extern u16 D_801956B0, D_801956B2, D_801956B4;
extern u8 D_801956A8[];
extern u16 D_800E11EA, D_800E2850[];
extern u16 D_800F3368, D_800F336A, D_800F336C, D_800F336E;
extern u16 D_800F3370, D_800F3372, D_800F3374, D_800F3376, D_800F3378;
extern int func_801945EC(int, RoomM123Particle *);
extern int func_800CE560(void *, int, int, int (*)(int, RoomM123Particle *));
extern RoomM123Particle *func_800CE610(void *);
extern void func_800CE8F0(void *, int, GteShortVector *, GteShortVector *);
extern int func_80071A54(void);
extern int func_80077CF4(int);
extern void func_800CE9D4(void *, int, void *);

int func_80194768(int mode)
{
    GteShortVector seed = D_8018F1E0;
    GteShortVector output;
    RoomM123Particle *particle;
    int i;
    int time;
    int scale;
    u16 outX, outY, outZ;
    RoomM123Context *context;
    void *texture;
    u16 palette;

    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->pool, 8, 24, func_801945EC);
    case 1:
        if (D_800E27EC == 1) {
            for (i = 0; i < 24; i++) {
                particle = func_800CE610(D_800F33E0->pool);
                if (particle != 0) {
                    particle->frame = (i << 6) / 24;
                    particle->scale = (func_80071A54() & 63) + 64;
                    particle->offset = func_80071A54();
                }
            }
        } else {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->frame = 0;
                particle->scale = (func_80071A54() & 63) + 64;
                particle->offset = func_80071A54();
            }
        }
        time = D_800E27EC;
        D_801956A4 = 0x1000;
        if (time >= 50)
            D_801956A4 = 0x1000 - ((time - 50) << 12) / 30;
        if (time < 80) goto ret0;
        return 1;
    case 2:
        func_800CE8F0(D_800F32D0->pool, 23, &seed, &output);
        scale = func_80077CF4((D_800E27EC << 11) / 80);
        if (scale < 0)
            scale += 31;
        outX = output.x;
        outY = output.y;
        outZ = output.z;
        context = D_800F32D0;
        /* Keep the context load ahead of the texture address. */
        asm volatile("" : : "r"(context) : "memory");
        texture = D_801956A8;
        D_801956B0 = outX;
        D_801956B2 = outY;
        D_801956B4 = outZ;
        /* Finish the position writes before reducing the signed scale. */
        asm volatile("" : : : "memory");
        D_801956A0 = scale >> 5;
        func_800CE9D4(context->pool, 0, texture);
        D_800F3368 = 16;
        D_800F336A = 1;
        D_800F3376 = 16;
        D_800F3378 = 16;
        palette = D_800E2850[D_800E11EA];
        /* Keep the palette lookup before the remaining effect-register writes. */
        asm volatile("" : : "r"(palette) : "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 12;
        D_800F3370 = palette;
        break;
    }
ret0:
    return 0;
}
