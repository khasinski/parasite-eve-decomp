#include "pe1/battle_runtime.h"

void Battle_SlotFree(void **combatantPtr) {
    u8 index;

    index = 0;
    do {
        if (&D_800A5D58[index].combatant == *combatantPtr) {
            D_800A5D58[index].active = 0;
        }
        index++;
    } while (index < 7);
    *combatantPtr = 0;
}
void Battle_ClearMotionTable(void) {
    u8 i;
    i = 0;
    do {
        D_800A5D58[i].active = 0;
        i++;
    } while (i < 7);
}
