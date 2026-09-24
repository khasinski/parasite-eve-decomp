#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"

typedef struct RoomPulseParticle {
    u16 x, y, z, pad;
    s16 state, frame;
} RoomPulseParticle;

extern GteShortVector D_8018F1F0;
extern GteMatrix *D_800BCFA4;
extern int D_800E27EC;
extern void func_800CF3AC(void *, void *, int);
extern int func_80077CF4(int);
extern void func_800D0728(void *, int, int, int, void *, int, int, int, void *, int, int);

int func_80194A70(int mode, RoomPulseParticle *particle, int *reference)
{
    int color[2];
    GteShortVector position = D_8018F1F0;
    GteShortVector *positionPtr;
    GteMatrix **matrixSlot;
    int wave;

    positionPtr = &position;
    switch (mode) {
    case 1:
        if (particle->state != 0)
            break;
        particle->frame++;
        if (particle->frame >= 24)
            return 1;
        break;
    case 2:
        if (particle->state != 0)
            break;
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800CF3AC((void *)*reference, color, particle->frame);
        wave = func_80077CF4((D_800E27EC << 10) / 24);
        func_800D0728(particle, 1000, 1500, 24, positionPtr, wave, wave, 0, color, 128, 1);
        break;
    }
    return 0;
}
