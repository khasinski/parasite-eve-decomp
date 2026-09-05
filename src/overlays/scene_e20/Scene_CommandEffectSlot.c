#include "pe1/random.h"

typedef struct SceneEffectSlot {
    short position[3];
    short pad06;
    short target[3];
    short pad0E;
    int duration;
    int pending;
} SceneEffectSlot;

extern SceneEffectSlot D_80190860[];
extern short D_800942EC;

SceneEffectSlot *Scene_CommandEffectSlot(unsigned int command, int index, int value, int z) {
    SceneEffectSlot *slot = &D_80190860[index];
    int random;

    slot->pending = 1;
    switch (command) {
    case 1:
        slot->target[0] = value;
        random = Engine_Random();
        slot->target[1] = D_800942EC - 600;
        slot->target[1] -= random & 0x1FF;
        slot->target[2] = z;
        slot->pending = 0;
        break;
    case 2:
        slot->position[0] = value;
        /* Keep the position store before the shared-height load; emits no code. */
        __asm__("");
        slot->position[2] = z;
        slot->position[1] = D_800942EC;
        break;
    case 3:
        slot->target[0] = value;
        random = Engine_Random();
        slot->target[1] = D_800942EC - 600;
        slot->target[1] -= random & 0x1FF;
        slot->target[2] = z;
        break;
    case 4:
        slot->duration = value;
        break;
    }
    return slot;
}
