/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"
static inline u32 *AllocateDrawMode(int mode) {
    u32 *packet = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        packet = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (packet) SetDrawMode((char *)packet, 0, 0, (mode & 3) << 5);
    return packet;
}

void Draw_EmitWipeBar(u8 *edges, int mode) {
    u32 *first = AllocateDrawMode(mode + 1);
    u32 *second = AllocateDrawMode(2 - mode);
    while ((s8)edges[0] >= 0) {
        Draw_AllocColorRect((s8)edges[0], (s8)edges[1], 2, mode);
        edges += 2;
    }
    ++edges;
    {
        u32 mask24 = 0xffffff, maskTop = 0xff000000;
        u32 *ot = g_DrawOrderingTableEntry;
        *first = (*first & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)first & mask24);
    }
    while ((s8)edges[0] >= 0) {
        Draw_AllocColorRect((s8)edges[0], (s8)edges[1], -2, !mode);
        edges += 2;
    }
    {
        u32 mask24 = 0xffffff, maskTop = 0xff000000;
        u32 *ot = g_DrawOrderingTableEntry;
        *second = (*second & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)second & mask24);
    }
}
