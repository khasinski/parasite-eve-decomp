/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_widget.h"

typedef struct CursorPair {
    u32 x;
    u32 y;
} CursorPair;

extern u32 D_8009D124, D_8009D128;
extern int D_8009D10C;
extern CursorPair *D_8009D12C;
extern CursorPair D_800A2270[], D_800A22B0[];
void BoundsCheck_AssertStub(int);
int VSync(int);
void Draw_AllocColorTri(int, int, int);

static inline void SetCursor(u32 x, u32 y)
{
    D_8009D124 = x;
    D_8009D128 = y;
}

static inline void MoveCursor(u32 dx, u32 dy)
{
    SetCursor(D_8009D124 + dx, D_8009D128 + dy);
}

static inline void PopCursor(void)
{
    CursorPair *stack = D_8009D12C;
    if ((u32)D_800A2270 < (u32)stack) {
        D_8009D12C = stack - 1;
        SetCursor(stack[-1].x, stack[-1].y);
    } else {
        BoundsCheck_AssertStub(3);
    }
}

void MenuWidget_DrawListRow(MenuWidgetNode *node,
                            void (*draw_callback)(int),
                            int row, int draw_cursor)
{
    u32 index = (u32)node->x_limit * ((u32)row + node->scroll_y);
    /* Stock MIPS GCC emits SLLV, which masks the shift count to five bits. */
    u32 bit = 1u << index;
    int x, enabled, dimmed;
    CursorPair *stack = D_8009D12C;
    int (*select_callback)(int);

    if ((u32)stack < (u32)D_800A22B0) {
        D_8009D12C = stack + 1;
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
            (x != node->cursor_x || (u32)row + node->scroll_y != (u32)node->cursor_y)))
            dimmed = 1;
        D_8009D10C = dimmed;
        if (draw_callback)
            draw_callback((u32)node->grid_width * ((u32)node->scroll_y + row) + x);
        if ((VSync(-1) & 8) && x == node->target_x &&
            (u32)row + node->scroll_y == (u32)node->target_y) {
            D_8009D124 -= 2;
            D_8009D128 -= 2;
            Draw_AllocColorTri(node->draw_state, node->disabled, 0);
            D_8009D124 += 2;
            D_8009D128 += 2;
        }
        MoveCursor(node->draw_state, 0);
    }
    PopCursor();
    MoveCursor(0, node->disabled);
}
