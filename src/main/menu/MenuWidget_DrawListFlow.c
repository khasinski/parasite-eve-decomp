/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_inventory.h"
#include "pe1/menu_draw_list.h"
#include "pe1/draw_state.h"
#include "pe1/draw_area.h"

void BoundsCheck_AssertStub(int);
int VSync(int);
void Draw_AllocColorTri(int, int, int);

typedef DrawTextCursorPair CursorPair;
extern CursorPair D_800A2270[], D_800A22B0[];

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

static inline GpuCmdPacket *AllocRect(DrawAreaRect *rect)
{
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + sizeof(GpuCmdPacket);
    GpuCmdPacket *packet = 0;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        packet = (GpuCmdPacket *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (packet) SetDrawArea(packet, rect);
    return packet;
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

static inline void RowMoveCursor(u32 dx, u32 dy)
{
    D_8009D124 = D_8009D124 + dx;
    D_8009D128 = D_8009D128 + dy;
}

static inline void RowPopCursor(void)
{
    CursorPair *stack = (CursorPair *)g_TextCursorStack;
    if ((u32)D_800A2270 < (u32)stack) {
        g_TextCursorStack = (int *)(stack - 1);
        D_8009D124 = stack[-1].x;
        D_8009D128 = stack[-1].y;
    } else {
        BoundsCheck_AssertStub(3);
    }
}

void MenuWidget_DrawListRow(MenuWidgetNode *node,
                            void (*draw_callback)(int),
                            int row, int draw_cursor)
{
    u32 index = node->x_limit * (row + node->scroll_y);
    /* Stock MIPS GCC emits SLLV, which masks the shift count to five bits. */
    u32 bit = 1u << index;
    int x, enabled, dimmed;
    CursorPair *stack = (CursorPair *)g_TextCursorStack;
    int (*select_callback)(int);

    if ((u32)stack < (u32)D_800A22B0) {
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = D_8009D124;
        stack->y = D_8009D128;
    } else {
        BoundsCheck_AssertStub(2);
    }
    D_8009D124 += 2;
    D_8009D128 += 2;
    for (x = 0; x < node->grid_width; x++) {
        select_callback = node->selectionAvailable;
        enabled = 1;
        if (select_callback) {
            enabled = select_callback(index++);
            node->cell_mask &= ~bit;
            if (enabled) node->cell_mask |= bit;
            bit <<= 1;
        }
        dimmed = 0;
        if (!enabled || (draw_cursor &&
            (x != node->cursor_x || row + node->scroll_y != node->cursor_y)))
            dimmed = 1;
        D_8009D10C = dimmed;
        if (draw_callback)
            draw_callback(node->grid_width * (node->scroll_y + row) + x);
        if ((VSync(-1) & 8) && x == node->target_x &&
            row + node->scroll_y == node->target_y) {
            D_8009D124 -= 2;
            D_8009D128 -= 2;
            Draw_AllocColorTri(node->draw_state, node->disabled, 0);
            D_8009D124 += 2;
            D_8009D128 += 2;
        }
        RowMoveCursor(node->draw_state, 0);
    }
    RowPopCursor();
    RowMoveCursor(0, node->disabled);
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

static inline void SetCursor(u32 x, u32 y)
{
    D_8009D124 = x;
    D_8009D128 = y;
}

static inline void MoveCursor(u32 dx, u32 dy)
{
    SetCursor(D_8009D124 + dx, D_8009D128 + dy);
}

static inline void PushCursor(void)
{
    int *stack = g_TextCursorStack;
    if (stack < g_TextCursorStackTop) {
        /* Capture both coordinates before writing into the shared stack. */
        int x = D_8009D124, y = D_8009D128;
        g_TextCursorStack = stack + 2;
        stack[0] = x;
        stack[1] = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

static inline void PopCursor(void)
{
    int *stack = g_TextCursorStack;
    if (g_TextCursorStackBottom < stack) {
        g_TextCursorStack = stack - 2;
        SetCursor(stack[-2], stack[-1]);
    } else {
        BoundsCheck_AssertStub(3);
    }
}


/* Coordinate offsets use the low 32 bits of the original MIPS arithmetic. */
static inline u32 Product(u32 size, u32 count)
{
    return size * count;
}

static inline u32 Difference(u32 a, u32 b)
{
    return a - b;
}

void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw_callback)(int))
{
    MenuWidgetNode *parent = FindOwner(node);
    /* Snapshot once: callbacks may change the parent's draw state. */
    int draw_cursor = parent->draw_state;
    DrawAreaRect rect;
    GpuCmdPacket *packet;
    int row, offset, highlight;
    D_8009D164 = node->draw_state;
    D_8009D168 = node->disabled;
    rect.x = 0; rect.y = 0; rect.w = 320; rect.h = 224;
    if (D_8009D108) rect.y = 224;
    packet = AllocRect(&rect);
    Gpu_LinkDrawArea(D_8009D11C, packet);
    PushCursor();
    MoveCursor(0, node->scroll_adjust);
    if (node->scroll_adjust > 0) {
        MoveCursor(0, 0u - node->disabled);
        MenuWidget_DrawListRow(node, draw_callback, -1, draw_cursor);
    }
    /* Row callbacks may change the viewport; reload its bounds each time. */
    for (row = 0; row < node->visible_rows; row++)
        MenuWidget_DrawListRow(node, draw_callback, row, draw_cursor);
    if (node->scroll_adjust < 0)
        MenuWidget_DrawListRow(node, draw_callback, row, draw_cursor);
    PopCursor();
    Draw_AllocPrimWithMask(node);
    MenuWidget_EaseNodePosition(node->popup_node);
    offset = node->scroll_adjust;
    if (offset) {
        if (offset > 0) {
            node->scroll_adjust = offset - node->disabled / 2;
            if (node->scroll_adjust < 0) node->scroll_adjust = 0;
        } else {
            node->scroll_adjust = offset + node->disabled / 2;
            if (node->scroll_adjust > 0) node->scroll_adjust = 0;
        }
        if (node->scroll_adjust) return;
    }
    if (node->cursor_x >= 0 && node->cursor_y >= node->scroll_y &&
        node->cursor_y < node->scroll_y + node->visible_rows) {
        PushCursor();
        D_8009D124 += Product(node->draw_state, node->cursor_x);
        D_8009D128 += Product(node->disabled, Difference(node->cursor_y, node->scroll_y));
        highlight = 0;
        if (!(node->layout_flags & 0x80) && g_MenuWidgetCurrentNode == node) {
            int blink = D_8009D0E8 && (Draw_RemapStatusFlags() & 0x20);
            if (blink) highlight = 1;
        }
        Draw_AllocColorTriGradient(node->draw_state, node->disabled, highlight,
                                   g_MenuWidgetCurrentNode == node);
        PopCursor();
    }
}
