
#include "pe1/geom_state.h"

int Render_SetEntryVisible(int index, int enabled) {
    GeomEntry *entry = (GeomEntry *)((u8 *)g_GeomState + g_GeomState->entry_offset) + index;

    if (enabled != 0) {
        entry->flags |= 2;
    } else {
        entry->flags &= 0xFD;
    }
    return 0;
}
