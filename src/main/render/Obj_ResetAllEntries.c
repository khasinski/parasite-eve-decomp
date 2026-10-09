#include "pe1/geom_state.h"

extern GeomState * D_800B1624 __asm__("D_800B1624");

int Obj_ResetAllEntries(void) __asm__("func_800655D4");

int Obj_ResetAllEntries(void) {
    GeomState *state;
    register GeomStateAddress cursor asm("a0");
    u8 *entryBase;
    GeomEntry *renderEntries;
    unsigned int i;
    unsigned int count;
    unsigned int value100;
    unsigned int value1;
    int framePad[2];


    state = D_800B1624;
    cursor.state = D_800B1624;
    entryBase = cursor.bytes + state->ctrl_offset;
    renderEntries = (GeomEntry *)(cursor.bytes + state->entry_offset);
    count = state->entry_count;
    i = 0;

    if (count != 0) {
        value100 = 0x100;
        value1 = 1;
        cursor.bytes = entryBase;
        do {
            register unsigned int oldValue asm("v1") = (u8)cursor.ctrl->field4;
            u8 *indexedPtr = cursor.bytes + cursor.ctrl->slot_offset;
            unsigned int renderIndex;

            cursor.ctrl->field8 = value100;
            cursor.ctrl->fieldA = 0;
            cursor.ctrl->head.b.flags = value1;
            *(u32 *)&cursor.ctrl->field4 = oldValue;
            renderIndex = *indexedPtr;
            renderEntries[renderIndex].flags |= 2;
            i++;
            cursor.ctrl++;
        } while (i < count);
    }


    return 0;
}
