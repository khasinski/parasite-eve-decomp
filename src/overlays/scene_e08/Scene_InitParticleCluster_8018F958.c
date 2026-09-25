#include "common.h"
#include "pe1/random.h"
#include "scene_e08.h"
#include "scene_particle_slots.h"

void func_8018F958(int unused0, int unused1, SceneParticleCluster *slots) {
    unsigned int i;

    for (i = 0; i < 30; ++i) {
        {
            int randomValue = Engine_Random();
            slots->position[i].y = 0;
            slots->position[i].x = randomValue % 800 - 200;
        }
        slots->position[i].z = Engine_Random() % 600 - 300;
        slots->angle[i] = Engine_Random() % 4096;
        slots->counter[i] = 0;
        slots->field1FE[i] = 0;
        slots->active[i] = 0;
        slots->timer[i] = Engine_Random() % 50 + 2;

        if (i >= 4) {
            if ((Engine_Random() & 1) == 0) {
                slots->brightness[i] = 0xFF;
                slots->texture[i] = 0x4C;
                slots->mode[i] = 5;
                slots->fieldF0[i] = Engine_Random() % 256 + 0x200;
            } else {
                slots->brightness[i] = 0xFF;
                slots->texture[i] = 0x58;
                slots->mode[i] = 6;
                slots->fieldF0[i] = Engine_Random() % 512 + 0x300;
            }
        } else {
            slots->brightness[i] = 0x80;
            slots->texture[i] = 0x44;
            slots->mode[i] = 2;
            slots->fieldF0[i] = 0x7C;
        }
    }
    slots->count = 30;
    {
        void *volatile *sound = &D_800B0E64;
        if (*sound) {
            func_8006DF50(*sound, 0x608, 0, 0x80, 0x7F);
        }
    }
}
