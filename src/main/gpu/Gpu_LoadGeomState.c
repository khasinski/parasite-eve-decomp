#include "pe1/geom_state.h"

/* Local flat-halfword view of the GeomEntry record (this loader bulk-copies the
 * whole entry to a VRAM packet, so it reads every field as u16/u32 -- a view
 * that conflicts with GeomEntry's mixed s16/u16 typing, hence kept local). */
typedef struct Entry {
    u16 geom_screen;
    u16 half02;
    u16 half04;
    u16 half06;
    u16 half08;
    u16 half0A;
    u16 half0C;
    u16 half0E;
    u16 half10;
    u16 half12;
    u32 word14;
    u32 word18;
    u32 word1C;
} Entry;

extern int g_RenderStateFlags;
extern u8 g_GeomGroupSel;                  /* active mesh-entry GROUP selector: only
                                       * entries with entry[+0x24]==this are drawn
                                       * (one bg layer-set / camera view) */
extern int *D_800BCFA8;
extern u16 * volatile g_GeomVramPacketDst;

void SetGeomScreen(int h);

int Gpu_LoadGeomState(int index) {
    Entry *entry;
    u16 *dst;
    int value;

    entry = (Entry *)((u8 *)g_GeomState + g_GeomState->entry_offset_1C + (index * 52));
    *D_800BCFA8 = entry->geom_screen;
    SetGeomScreen(entry->geom_screen);

    dst = g_GeomVramPacketDst;
    dst[0] = entry->half02;
    dst = g_GeomVramPacketDst;
    dst[1] = entry->half04;
    dst = g_GeomVramPacketDst;
    dst[2] = entry->half06;
    dst = g_GeomVramPacketDst;
    dst[3] = entry->half08;
    dst = g_GeomVramPacketDst;
    dst[4] = entry->half0A;
    dst = g_GeomVramPacketDst;
    dst[5] = entry->half0C;
    dst = g_GeomVramPacketDst;
    dst[6] = entry->half0E;
    dst = g_GeomVramPacketDst;
    dst[7] = entry->half10;
    dst = g_GeomVramPacketDst;
    dst[8] = entry->half12;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[5] = entry->word14;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[6] = entry->word18;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[7] = entry->word1C;

    g_GeomGroupSel = index;
    value = g_RenderStateFlags;
    g_RenderStateFlags = value | 0x80;
    return 0;
}
