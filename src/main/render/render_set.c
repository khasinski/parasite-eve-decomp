#include "pe1/geom_state.h"

extern volatile int g_RenderStateFlags;

int Render_SetEntryVisible(int index, int enabled) {
    GeomEntry *entry = (GeomEntry *)((u8 *)g_GeomState + g_GeomState->entry_offset) + index;

    if (enabled != 0) {
        entry->flags |= 2;
    } else {
        entry->flags &= 0xFD;
    }
    return 0;
}

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
