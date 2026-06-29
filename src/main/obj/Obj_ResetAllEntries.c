/* CC1_FLAGS: -G8 */

#include "pe1/geom_state.h"

int Obj_ResetAllEntries(void) {
    GeomState *state;
    register u8 *cursor asm("a0");
    u8 *entryBase;
    u8 *renderEntries;
    register unsigned int i asm("a1");
    register unsigned int count asm("a2");
    register unsigned int value100 asm("t2");
    register unsigned int value1 asm("t1");

    asm("addiu   $sp,$sp,-8");
    state = g_GeomState;
    cursor = (u8 *)g_GeomState;
    entryBase = cursor + state->ctrl_offset;
    renderEntries = cursor + state->entry_offset;
    count = state->entry_count;
    i = 0;

    if (count != 0) {
        value100 = 0x100;
        value1 = 1;
        cursor = entryBase;
        do {
            register unsigned int oldValue asm("v1") = *(u8 *)(cursor + 4);
            register u8 *indexedPtr asm("v0") = cursor + *(int *)(cursor + 12);
            unsigned int renderIndex;

            *(u16 *)(cursor + 8) = value100;
            *(u16 *)(cursor + 10) = 0;
            *cursor = value1;
            *(int *)(cursor + 4) = oldValue;
            renderIndex = *indexedPtr;
            renderEntries[renderIndex * 56] |= 2;
            i++;
            cursor += 16;
        } while (i < count);
    }

    {
        register int ret asm("v0");

        asm("addu    %0,$zero,$zero" : "=r"(ret));
        asm("addiu   $sp,$sp,8");
        return ret;
    }
}
