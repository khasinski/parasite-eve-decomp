#include "common.h"
#include "pe1/random.h"

typedef struct SceneParticleOffset {
    s16 x;
    s16 y;
    s16 z;
    s16 pad06;
} SceneParticleOffset;

typedef struct SceneParticleSlots {
    unsigned char pad00[8];
    SceneParticleOffset offset[4];
    s32 phase[4];
    s16 verticalOffset[4];
    unsigned char pad40[8];
    s16 timer[4];
    unsigned char pad50[8];
    s16 activeCount;
    s16 completedCount;
} SceneParticleSlots;

void func_80191DC0(int unused0, int unused1, SceneParticleSlots *slots) {
    unsigned int i;

    slots->activeCount = 0;
    slots->completedCount = 0;
    for (i = 0; i < 4; i++) {
        slots->phase[i] = i << 11;
        slots->verticalOffset[i] = 0;
        slots->offset[i].x = 0;
        slots->offset[i].y = 0;
        slots->offset[i].z = 0;
        slots->timer[i] = Engine_Random() % 10 + 10;
    }
}
