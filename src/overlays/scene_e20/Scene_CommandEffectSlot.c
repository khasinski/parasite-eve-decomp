#include "pe1/random.h"

#include "pe1/scene_e20_hover_orb.h"

SceneEffectSlot *Scene_CommandEffectSlot(unsigned int command, int index, int value, int z) {
    SceneEffectSlot *slot = &D_80190860[index];
    int random;

    slot->pending = 1;
    switch (command) {
    case 1:
        slot->target.x = value;
        random = Engine_Random();
        slot->target.y = D_800942EC.height - 600;
        slot->target.y -= random & 0x1FF;
        slot->target.z = z;
        slot->pending = 0;
        break;
    case 2:
        slot->position.x = value;
        /* Keep the position store before the shared-height load; emits no code. */
        __asm__("");
        slot->position.y = D_800942EC.height;
        slot->position.z = z;
        break;
    case 3:
        slot->target.x = value;
        random = Engine_Random();
        slot->target.y = D_800942EC.height - 600;
        slot->target.y -= random & 0x1FF;
        slot->target.z = z;
        break;
    case 4:
        slot->duration = value;
        break;
    }
    return slot;
}
