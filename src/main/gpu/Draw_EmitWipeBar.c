/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"
void Draw_EmitWipeBar(u8 *edges, int mode) {
    u32 *first = 0;
    int firstMode = mode + 1;
    u32 *second;
    int secondMode;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        first = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (first) SetDrawMode((char *)first, 0, 0, (firstMode & 3) << 5);
    secondMode = 2 - mode;
    /* Finish selecting the second mode before beginning its allocation. */
    asm("" : : "r"(secondMode));
    second = 0;
    old = g_DrawPacketCursor;
    next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        second = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (second) SetDrawMode((char *)second, 0, 0, (secondMode & 3) << 5);
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
