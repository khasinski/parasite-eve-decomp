/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/draw_area.h"

static inline MenuWidgetNode *FindOwner(MenuWidgetNode *node)
{
    MenuWidgetNode *found = 0;
    MenuWidgetNode *scan = g_MenuWidgetActiveListHead;
    while (scan) {
        int i;
        for (i = 0; i < 4; i++) {
            if (scan->children[i] == node) break;
        }
        if (i < 4) {
            found = scan;
            break;
        }
        scan = scan->next;
    }
    return found;
}

static inline void GetPosition(MenuWidgetNode *node, u32 *x, u32 *y)
{
    *y = 0;
    *x = 0;
    while (node) {
        *x += node->x;
        *y += node->y;
        node = FindOwner(node);
    }
}

static inline GpuCmdPacket *AllocRect(DrawAreaRect *rect)
{
    u8 *old, *next;
    GpuCmdPacket *packet = 0;
    old = g_DrawPacketCursor;
    next = old + sizeof(GpuCmdPacket);
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        packet = (GpuCmdPacket *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet) SetDrawArea(packet, rect);
    return packet;
}

static inline GpuCmdPacket *AllocArea(u32 x, u32 y, u32 width, u32 height)
{
    DrawAreaRect rect;
    if (D_8009D108) y += 0xE0;
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    return AllocRect(&rect);
}

static inline u32 Dimension(u32 size, u32 count)
{
    return size * count;
}

void Draw_AllocPrimWithMask(MenuWidgetNode *node)
{
    u32 x, y;
    GpuCmdPacket *packet;
    GetPosition(node, &x, &y);
    packet = AllocArea(x, y, Dimension(node->draw_state, node->grid_width),
                      Dimension(node->disabled, node->visible_rows));
    Gpu_LinkDrawArea(g_DrawOrderingTableEntry, packet);
}
