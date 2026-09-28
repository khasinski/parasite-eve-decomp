#include "RoomEffect_Room188390.h"

extern int D_800E27EC;
extern u8 *D_800E2368;
extern RoomEffectPoolContext *D_800F32D0, *D_800F33E0;
extern u16 D_800E11EA, D_800E2850[];
extern u16 D_800F336C, D_800F336E, D_800F3370, D_800F3372, D_800F3374;
extern int func_80193BCC(int, RoomEffectTimedParticle *);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern int func_800CE560(void *, int, int, int (*)(int, RoomEffectTimedParticle *));
extern RoomEffectTimedParticle *func_800CE610(void *);

int func_80194588(int mode, s16 *counter)
{
    /* Retail reads these three stack halfwords without writing them here. */
    u16 seed[3];
    RoomEffectTimedParticle *particle;

    switch (mode) {
    case 0:
        *counter = 0;
        func_800D3F64(0x594, func_800D3FD8());
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
        return func_800CE560(D_800F33E0->pool, 24, 14, func_80193BCC);
    case 1:
        if (*counter < 11) {
            particle = func_800CE610(D_800F33E0->pool);
            if (particle != 0) {
                particle->x = seed[0];
                particle->y = seed[1];
                particle->z = seed[2];
                particle->phase = 0;
                particle->frame = *counter;
                if (*counter >= 10) particle->active = 1;
                else particle->active = 0;
            }
            ++*counter;
        }
        if (D_800E27EC < 10) goto ret0;
        return 2;
    case 2: {
        u16 palette = D_800E2850[D_800E11EA];
        /* Keep the palette lookup before the effect-register stores. */
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
