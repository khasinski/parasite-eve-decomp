
#include "pe1/geom_state.h"

int Render_SetEntryScrolled(int index, int enabled, unsigned int arg2, unsigned int arg3) {
    GeomEntry *entry = (GeomEntry *)((u8 *)g_GeomState + g_GeomState->entry_offset) + index;

    if (enabled != 0) {
        entry->flags |= 4;
    } else {
        entry->flags &= 0xFB;
    }
    entry->field1C = arg2 >> 8;
    entry->field1E = arg3 >> 8;
    return 0;
}
