
#include "pe1/geom_state.h"

int Render_SetEntryMirrored(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomEntry *entry = (GeomEntry *)((u8 *)g_GeomState + g_GeomState->entry_offset) + index;

    if (enabled != 0) {
        entry->flags |= 8;
    } else {
        entry->flags &= 0xF7;
    }
    entry->field1C = (0x10000 - arg2) >> 8;
    entry->field1E = (0x10000 - arg3) >> 8;
    return 0;
}
