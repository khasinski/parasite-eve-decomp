#include "common.h"
#include "pe1/field_engine_state.h"

/* Activate slot `slot` with handler `id` and reserve `size` bytes of script
 * data for it, wrapping to offset 0 when the 0x80C-byte area would overflow. */
void func_800C2D0C(u16 slot, u8 id, int size) {
    FieldEngDataState *state;
    s16 offset;

    g_FieldEngineSlots[slot].flag = 1;
    g_FieldEngineSlots[slot].handler_id = id;
    g_FieldEngineSlots[slot].counter = 0;
    state = g_FieldEngineState;
    offset = state->data_next;
    if ((unsigned int)(offset + (u16)size) >= 0x80C) {
        offset = 0;
    }
    g_FieldEngineSlots[slot].data_offset = offset;
    offset += size;
    state->data_next = offset;
    state->slot_count++;
}
