#include "common.h"
#include "pe1/random.h"
#include "scene_e08.h"
#include "scene_particle_slots.h"

typedef struct SceneActorPositionTail {
    u8 pad00[0x14];
    s32 x;
    s32 y;
    s32 z;
} SceneActorPositionTail;

void func_801902CC(int unused0, int unused1, SceneEmitterSlots *slots) {
    unsigned int i;
    SceneActorPositionTail *origin = (SceneActorPositionTail *)((s32 *)func_800C2B50() + 1);

    slots->activeSlots = 4;
    for (i = 0; i < 4; ++i) {
        slots->field3C[i] = 0;
        {
            int randomValue = Engine_Random();
            int center = origin->x - 100;
            slots->position[i].x = center + randomValue % 200;
        }
        slots->position[i].y = origin->y;
        {
            int randomValue = Engine_Random();
            int center = origin->z - 100;
            slots->position[i].z = center + randomValue % 200;
        }
        slots->field40[i] = Engine_Random() % 5 + 1;
        slots->field20[i] = Engine_Random() % 256 + 0x400;
        slots->field30[i] = 0x48;
        slots->field34[i] = 4;
        slots->field28[i] = 0x40;
        slots->field38[i] = 0;
    }
}
