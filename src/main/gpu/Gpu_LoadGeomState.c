#include "pe1/geom_state.h"

extern int g_RenderStateFlags;
extern u8 g_GeomGroupSel;                  /* active mesh-entry GROUP selector: only
                                       * entries with entry[+0x24]==this are drawn
                                       * (one bg layer-set / camera view) */
extern int *D_800BCFA8;
extern u16 * volatile g_GeomVramPacketDst;

void SetGeomScreen(int h);

int Gpu_LoadGeomState(int index) {
    GeomStateAddress table, base;
    CameraViewport *entry;
    u16 *dst;
    int value;

    GEOM_STATE_OFFSET(table, base, entry_offset_1C, index * 52);
    entry = table.viewport;
    *D_800BCFA8 = entry->prefix.gpu.geom_screen;
    SetGeomScreen(entry->prefix.gpu.geom_screen);

    dst = g_GeomVramPacketDst;
    dst[0] = entry->prefix.gpu.half02;
    dst = g_GeomVramPacketDst;
    dst[1] = entry->prefix.gpu.half04;
    dst = g_GeomVramPacketDst;
    dst[2] = entry->prefix.gpu.half06;
    dst = g_GeomVramPacketDst;
    dst[3] = entry->prefix.gpu.half08;
    dst = g_GeomVramPacketDst;
    dst[4] = entry->prefix.gpu.half0A;
    dst = g_GeomVramPacketDst;
    dst[5] = entry->prefix.gpu.half0C;
    dst = g_GeomVramPacketDst;
    dst[6] = entry->prefix.gpu.half0E;
    dst = g_GeomVramPacketDst;
    dst[7] = entry->prefix.gpu.half10;
    dst = g_GeomVramPacketDst;
    dst[8] = entry->prefix.gpu.half12;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[5] = entry->prefix.gpu.word14;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[6] = entry->prefix.gpu.word18;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[7] = entry->prefix.gpu.word1C;

    g_GeomGroupSel = index;
    value = g_RenderStateFlags;
    g_RenderStateFlags = value | 0x80;
    return 0;
}
