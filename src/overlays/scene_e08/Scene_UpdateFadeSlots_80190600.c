#include "common.h"

typedef struct SceneFadeSlots {
    unsigned char pad00[0x28];
    s16 intensity[4];
    unsigned char pad30[8];
    u8 frame[4];
    u8 state[4];
    u8 delay[4];
    u8 activeCount;
} SceneFadeSlots;

void func_80190600(int unused, u8 *sceneState, SceneFadeSlots *slots) {
    unsigned int i;
    u8 frame;

    for (i = 0; i < 4; i++) {
        if (slots->delay[i] != 0) {
            slots->delay[i]--;
            if (slots->delay[i] == 0) {
                slots->state[i] = 1;
            }
        }

        if (slots->state[i] == 1) {
            if (slots->intensity[i] >= 0x11) {
                slots->intensity[i] -= 0x10;
            }
            frame = slots->frame[i];
            slots->frame[i] = frame + 1;
            if (frame == 8) {
                slots->state[i] = 0;
                slots->activeCount--;
                if (slots->activeCount == 0) {
                    sceneState[1] = 2;
                }
            }
        }
    }
}
