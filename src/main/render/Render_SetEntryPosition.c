
#include "pe1/geom_state.h"

extern volatile int g_RenderStateFlags;

int Render_SetEntryPosition(int index, int x, int y) {
    volatile GeomEntry *entry = (GeomEntry *)((u8 *)g_GeomState + g_GeomState->entry_offset) + index;
    volatile int *flags = &g_RenderStateFlags;

    entry->scr_x = x;
    entry->base_x = x;
    entry->scr_y = y;
    entry->base_y = y;
    *flags |= 0x80;
    return 0;
}
