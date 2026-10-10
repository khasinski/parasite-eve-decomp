/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_widget.h"
#include "pe1/draw_state.h"
#include "pe1/menu_scroll_cursor.h"

/* Contiguous widget construction, pool initialization, list drawing and layout. */
#include "pe1/bounds_check.h"
MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void);


MenuWidgetNode *g_MenuWidgetCurrentNode;

MenuWidgetNode *g_MenuWidgetSavedNode;

MenuWidgetNode *g_MenuWidgetActiveListHead;

void MenuWidget_SetCurrentNode(MenuWidgetNode *node) {
    g_MenuWidgetCurrentNode = node;
}

MenuWidgetNode *MenuWidget_GetCurrentNode(void) {
    return g_MenuWidgetCurrentNode;
}

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0) {
    g_MenuWidgetSavedNode = g_MenuWidgetCurrentNode;
    g_MenuWidgetCurrentNode = arg0;
}

void MenuWidget_RestoreSavedCurrentNode(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *pending;

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        pending = g_MenuWidgetSavedNode;
        while (node != 0) {
            if (node == pending) {
                break;
            }
            node = node->next;
        }
        if (node != 0) {
            g_MenuWidgetCurrentNode = node;
        }
    }
    g_MenuWidgetSavedNode = 0;
}

MenuWidgetNode *MenuWidget_CreateSimpleNode(s32 arg0, MenuWidgetNode *arg1, MenuWidgetNode *arg2, s32 arg3) {
    s32 mode = arg0;
    register MenuWidgetNode *parent_arg asm("$19") = arg1;
    MenuWidgetNode *parent = arg2;
    s32 arg_flag = arg3;
    MenuWidgetSimpleDescriptor *desc;
    MenuWidgetNode *node;
    MenuWidgetNode *next;
    MenuWidgetNode *old_head;

    desc = MenuWidget_LookupSimpleDescriptor(mode);
    if (desc == 0) {
        BoundsCheck_AssertStub(0xC);
    }

    node = g_MenuWidgetFreeListHead;
    if (node == 0) {
        BoundsCheck_AssertStub(0xA);
    }

    next = node->next;
    old_head = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    node->parent = parent_arg;
    node->update = 0;
    node->draw = 0;
    g_MenuWidgetFreeListHead = next;
    node->next = old_head;

    {
        int i;
        register MenuWidgetNode **ptr asm("$5");

        i = 3;
        ptr = &node->children[1];
        do {
            ptr[2] = 0;
            i--;
            ptr--;
        } while (i >= 0);
    }

    node->y = 0;
    node->x = 0;
    node->selected_base = 0;
    node->mode = 0;
    node->flags = 0;

    if (parent != 0) {
        int i;
        register MenuWidgetNode **ptr asm("$3");

        i = 0;
        ptr = (MenuWidgetNode **)parent;
        while (i < 4 && ptr[2] != 0) {
            i++;
            ptr++;
        }
        if (i < 4) {
            parent->children[i] = node;
        } else {
            BoundsCheck_AssertStub(0xB);
        }
    }

    if (node == 0) {
        BoundsCheck_AssertStub(0xD);
    }

    if (arg_flag == 0) {
        MenuWidgetNode *found;

        found = MenuWidget_FindLastMode1WithCursorX();
        if (found != 0) {
            MenuWidgetNode *head;
            MenuWidgetNode *found_next;
            MenuWidgetNode *node_next;

            head = g_MenuWidgetActiveListHead;
            found_next = found->next;
            node_next = head->next;
            head->next = found_next;
            found->next = head;
            g_MenuWidgetActiveListHead = node_next;
        }
    }

    node->mode = 1;
    node->selected_base = mode;
    if (desc->x != 0) {
        node->x = desc->x;
        node->y = desc->y;
    } else {
        register int x_base asm("$2");
        int width;
        int half_h;
        int flag;

        width = desc->width;
        x_base = 0xA0;
        width >>= 1;
        x_base -= width;
        node->x = x_base;

        x_base = desc->height;
        flag = desc->y;
        half_h = x_base >> 1;
        if (flag != 0) {
            x_base = 0x50;
        } else {
            x_base = 0x78;
        }
        x_base -= half_h;
        node->y = x_base;
    }

    node->grid_width = desc->width;
    node->visible_rows = desc->height;
    node->draw_state = 0;
    node->disabled = 0;
    node->cursor_x = arg_flag;
    node->cursor_y = 0;
    node->appearance.cursorTargetX = 0;
    return node;
}

void MenuWidget_DestroyNode(MenuWidgetNode *node) {
    MenuWidget_DestroyNodeRecursive(node);
}

void MenuWidget_NavScrollTo(int selected_base) {
    MenuWidgetNode *node;
    int one;

    node = g_MenuWidgetActiveListHead;
    one = 1;
    while (node != 0) {
        if (node->mode == one && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }

    MenuWidget_DestroyNodeRecursive(node);
}

void MenuWidget_InitPool(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *end;
    MenuWidgetNode *next;

    node = g_MenuWidgetNodePool;
    end = node + 24;

    while (node < end) {
        next = node + 1;
        node->next = next;
        node = next;
    }

    g_MenuWidgetNodePoolSentinel.next = 0;
    g_MenuWidgetFreeListHead = g_MenuWidgetNodePool;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}

void Draw_PushPrimToList(void *arg0);

extern int g_DrawTextDimmed;
extern int g_TextCursorX;
extern int g_TextCursorY;
extern int g_DrawTextBoxWidth;

int g_TextCursorX;
int g_TextCursorY;

void MenuWidget_UpdateAndDraw(void) {
    MenuWidgetNode *node;
    int any_flag;
    int order_flag;

    node = g_MenuWidgetActiveListHead;
    order_flag = 0;
    any_flag = 0;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->draw_state = 0;
                any_flag |= node->cursor_x;
            }
            node = node->next;
        } while (node != 0);
    }

    node = g_MenuWidgetCurrentNode;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->draw_state = order_flag;
                order_flag = 1;
            }
            node = node->parent;
        } while (node != 0);
    }

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        do {
            int enabled;
            int extra;

            if (node->mode == 1 && node->cursor_y == 0) {
                if (node->disabled != 0) {
                    enabled = 0;
                } else {
                    enabled = node->draw_state & 1;
                }
                node->draw_state = enabled;

                extra = 0;
                if (any_flag != 0) {
                    extra = (unsigned int)node->cursor_x < 1;
                }

                node->draw_state = enabled | extra;
                g_DrawTextDimmed = enabled | extra;
                g_TextCursorX = 0;
                g_TextCursorY = 0;
                g_DrawTextBoxWidth = node->grid_width;
                Draw_PushPrimToList(node);
                g_TextCursorX = node->x;
                g_TextCursorY = node->y;
                g_DrawTextDimmed = node->draw_state;
                Draw_AllocColorGradient(node->grid_width, node->visible_rows, node->appearance.gradientPoints, node->cursor_x);
            }
            node = node->next;
        } while (node != 0);
    }
}

void MenuWidget_OffsetPosition(MenuWidgetNode *ptr, int dx, int dy) {
    if (ptr != 0) {
        ptr->x += dx;
        ptr->y += dy;
        g_TextCursorX += dx;
        g_TextCursorY += dy;
    }
}

void MenuWidget_ClearCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 0;
    }
}

void MenuWidget_SetCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 1;
    }
}

int MenuWidget_IsCursorYClear(MenuWidgetNode *ptr) {
    if (ptr) {
        return ptr->cursor_y == 0;
    }

    return 0;
}

MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        if (node->mode == 1 && node->cursor_x != 0) {
            result = node;
        }
        node = node->next;
    }
    return result;
}


MenuWidgetNode *MenuWidget_CreateNode(s32 arg0, MenuWidgetNode *arg1, MenuWidgetNode *arg2) {
    s32 mode = arg0;
    register MenuWidgetNode *parent_arg asm("$20") = arg1;
    MenuWidgetNode *parent = arg2;
    MenuWidgetGridDescriptor *desc;
    MenuWidgetNode *node;
    MenuWidgetNode *next;
    MenuWidgetNode *old_head;
    int tmp;

    desc = MenuWidget_LookupGridDescriptor(mode);
    if (desc == 0) {
        BoundsCheck_AssertStub(0xE);
    }

    node = g_MenuWidgetFreeListHead;
    if (node == 0) {
        BoundsCheck_AssertStub(0xA);
    }

    next = node->next;
    old_head = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    node->parent = parent_arg;
    node->update = 0;
    node->draw = 0;
    g_MenuWidgetFreeListHead = next;
    node->next = old_head;

    {
        int i;
        register MenuWidgetNode **ptr asm("$5");

        i = 3;
        ptr = &node->children[1];
        do {
            ptr[2] = 0;
            i--;
            ptr--;
        } while (i >= 0);
    }

    node->y = 0;
    node->x = 0;
    node->selected_base = 0;
    node->mode = 0;
    node->flags = 0;

    if (parent != 0) {
        int i;
        register MenuWidgetNode **ptr asm("$3");

        i = 0;
        ptr = (MenuWidgetNode **)parent;
        while (i < 4 && ptr[2] != 0) {
            i++;
            ptr++;
        }
        if (i < 4) {
            parent->children[i] = node;
        } else {
            BoundsCheck_AssertStub(0xB);
        }
    }

    if (node == 0) {
        BoundsCheck_AssertStub(0xF);
    }

    node->mode = 2;
    node->aux_index = mode;
    node->selected_base = mode;
    {
        int t;
        int u;

        t = desc->x;
        node->x = t;
        u = desc->y;
        t = (s32)Menu_StepScrollCursor;
        node->update = (void (*)())t;
        node->y = u;
        t = desc->gridWidth;
        node->x_limit = t;
        node->grid_width = t;
        t = desc->visibleRows;
        node->visible_rows_mirror = t;
        node->visible_rows = t;
        t = desc->yLimit;
        node->y_limit = t;
        t = desc->initialDrawState;
        node->draw_state = t;
        u = desc->initialDisabled;
        tmp = u;
    }
    node->cursor_y = 0;
    node->cursor_x = 0;
    node->target_y = -1;
    node->appearance.cursorTargetX = -1;
    node->scroll_y = 0;
    node->scroll_adjust = 0;
    node->disabled = tmp;
    tmp = desc->layoutFlags;
    node->linkedNext = 0;
    node->linkedPrevious = 0;
    node->itemAction = 0;
    node->refreshItems = 0;
    node->has_scroll = 0;
    node->layout_flags = tmp;
    node->selectionAvailable = 0;
    node->popup_node = 0;
    if (node->visible_rows < node->y_limit) {
        Draw_SwapPrimBuffers(node);
    }
    MenuWidget_ApplyColumnLayout(node);
    return node;
}

#include "pe1/menu_widget.h"

int MenuWidget_GridCellIndex(MenuWidgetNode *ptr) {
    int ret;
    int x;
    int y;

    ret = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            if (y >= 0) {
                ret = (ptr->grid_width * y) + x;
            }
        }
    }

    return ret;
}
#include "pe1/menu_widget.h"

int MenuWidget_GetCellIndex(MenuWidgetNode *ptr) {
    int idx;
    int ret;
    int x;
    int y;

    idx = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            idx = y;
            if (y >= 0) {
                idx = (ptr->grid_width * y) + x;
            } else {
                idx = -1;
            }
        }
    }

    ret = -1;
    if (((ptr->cell_mask >> idx) & 1) != 0) {
        ret = idx;
    }
    return ret;
}

#include "pe1/menu_inventory.h"
#include "pe1/menu_draw_list.h"
#include "pe1/draw_state.h"
#include "pe1/draw_area.h"

#include "pe1/bounds_check.h"
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
        if ((VSync(-1) & 8) && x == node->appearance.cursorTargetX &&
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
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        /* Capture both coordinates before writing into the shared stack. */
        int x = D_8009D124, y = D_8009D128;
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = x;
        stack->y = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

static inline void PopCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if ((DrawTextCursorPair *)g_TextCursorStackBottom < stack) {
        g_TextCursorStack = (int *)(stack - 1);
        SetCursor(stack[-1].x, stack[-1].y);
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

#include "pe1/menu_widget.h"

void MenuWidget_ClampScroll(MenuWidgetNode *ptr) {
    int y;
    int top;
    int height;
    int next;

    y = ptr->cursor_y;
    top = ptr->scroll_y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}
#include "pe1/menu_widget.h"

void MenuWidget_ClampCursor(MenuWidgetNode *ptr, int x, int y) {
    int limit;
    int top;
    int height;
    int next;

    if (x < 0) {
        x = 0;
    } else {
        limit = ptr->x_limit;
        if (x >= limit) {
            x = limit - 1;
        }
    }

    if (y < 0) {
        y = 0;
    } else {
        limit = ptr->y_limit;
        if (y >= limit) {
            y = limit - 1;
        }
    }

    top = ptr->scroll_y;
    ptr->cursor_x = x;
    ptr->cursor_y = y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}

#include "pe1/menu_scroll_cursor.h"

static inline int MenuWidget_CursorCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->cursor_x;
        if (x >= 0) {
            y = node->cursor_y;
            if (y >= 0)
                cell = node->grid_width * y + x;
        }
    }
    return cell;
}

static inline int MenuWidget_TargetCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->appearance.cursorTargetX;
        if (x >= 0 && (y = node->target_y) >= 0)
            cell = node->grid_width * y + x;
        else
            cell = -1;
    }
    return cell;
}

/* Rows past the last selectable one: column 1 of a list with a trailing
 * scroll row has one cell fewer. */
static inline int MenuWidget_EndMargin(MenuWidgetNode *node)
{
    return (node->cursor_x == 1 && node->has_scroll) + 1;
}

static inline void MenuWidget_ScrollToCursor(MenuWidgetNode *node)
{
    int y;
    int top;
    int height;

    y = node->cursor_y;
    top = node->scroll_y;
    if (y < top) {
        node->scroll_y = y;
    } else {
        height = node->visible_rows;
        if (y >= top + height)
            node->scroll_y = y - height + 1;
    }
}

static inline int MenuWidget_HeldShoulder(void)
{
    if (D_8009D0E8)
        return (Draw_RemapStatusFlags() & 0x5000) != 0;
    return 0;
}

/* Swaps the cursor cell of `node` with the marked cell of `other`. */
static inline int MenuWidget_SwapMarked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;
    int from;
    int to;

    done = 0;
    if (other != 0 && other->appearance.cursorTargetX >= 0) {
        from = MenuWidget_CursorCell(node);
        to = MenuWidget_TargetCell(other);
        if (node->selected_base != other->selected_base || from != to) {
            if (node->itemAction(node->selected_base, from, other->selected_base, to)) {
                other->appearance.cursorTargetX = -1;
                Menu_PlayConfirmSound();
                done = 1;
            } else {
                Menu_PlayErrorSound();
                done = 1;
            }
        }
    }
    return done;
}

/* Restores the marked cell of a linked list as its cursor and focuses it. */
static inline int MenuWidget_RestoreLinked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;

    done = 0;
    if (other != 0 && other->appearance.cursorTargetX >= 0) {
        other->cursor_x = other->appearance.cursorTargetX;
        other->appearance.cursorTargetX = -1;
        other->cursor_y = other->target_y;
        node->cursor_x = -1;
        MenuWidget_ScrollToCursor(other);
        g_MenuWidgetCurrentNode = other;
        done = 1;
    }
    return done;
}

/* Default update handler of the grid list widgets: moves the cursor with the
 * d-pad (crossing into the linked lists at the edges), scrolls, pages the
 * popup list, and swaps or restores marked cells. Returns nonzero when the
 * input was consumed. */
int Menu_StepScrollCursor(MenuWidgetNode *node, unsigned int buttons)
{
    MenuWidgetNode *link;
    int changed;
    int flags;
    int wrap;
    int limit;
    int row;
    int held;
    int margin;

    changed = 0;
    if (D_8009D0E8)
        flags = Draw_RemapStatusFlags();
    else
        flags = 0;
    if (flags & 0x20) {
        changed = 1;
    } else if (buttons & 0x1000) {
        if (node->cursor_y > 0) {
            node->cursor_y--;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            changed = 1;
            node->cursor_y = node->y_limit - 1;
            Menu_PlayMoveSound();
        }
        if (node->cursor_y < node->scroll_y && node->scroll_adjust == 0) {
            int old = node->scroll_y;
            int next = old - 1;

            node->scroll_y = next;
            if (next < 0)
                node->scroll_y = 0;
            else if (node->y_limit - node->visible_rows < next)
                node->scroll_y = node->y_limit - node->visible_rows;
            if (old != node->scroll_y)
                node->scroll_adjust = -node->disabled / 2;
        }
        changed |= !(node->layout_flags & 4);
    } else if (buttons & 0x4000) {
        wrap = 0;
        if (node->cursor_x)
            wrap = node->has_scroll != 0;
        margin = wrap + 1;
        if (node->cursor_y < node->y_limit - margin) {
            node->cursor_y++;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            node->cursor_y = 0;
            Menu_PlayMoveSound();
            changed = 1;
        }
        {
            int old = node->scroll_y;

            if (node->cursor_y >= old + node->visible_rows
                                      - (wrap && node->cursor_y == node->y_limit - 2)
                && node->scroll_adjust == 0) {
                int next = old + 1;

                node->scroll_y = next;
                if (next < 0)
                    node->scroll_y = 0;
                else if (node->y_limit - node->visible_rows < next)
                    node->scroll_y = node->y_limit - node->visible_rows;
                if (old != node->scroll_y)
                    node->scroll_adjust = node->disabled / 2;
            }
        }
        changed |= !(node->layout_flags & 8);
    } else if (buttons & 0x8000) {
        int x = node->cursor_x;

        if (x > 0) {
            node->cursor_x = x == 6 ? 4 : x - 1;
            Menu_PlayMoveSound();
            changed = 1;
        } else {
            link = node->linkedPrevious;
            if (link != 0) {
                int last;

                node->cursor_x = -1;
                limit = link->y_limit - 1;
                link->cursor_x = link->grid_width - 1;
                row = node->cursor_y - node->scroll_y + link->scroll_y;
                if (row < limit)
                    limit = row;
                last = 0;
                link->cursor_y = limit;
                if (link->has_scroll)
                    last = limit == link->y_limit - 1;
                changed = 1;
                g_MenuWidgetCurrentNode = link;
                margin = last + 1;
                link->cursor_x = link->grid_width - margin;
                Menu_PlayMoveSound();
            }
        }
        held = MenuWidget_HeldShoulder();
        if (!(node->layout_flags & 1))
            changed |= 1;
        else
            changed |= held;
    } else if (buttons & 0x2000) {
        int x = node->cursor_x;

        if (x >= 0) {
            int last;

            last = 0;
            if (node->cursor_y == node->y_limit - 1)
                last = node->has_scroll != 0;
            margin = last + 1;
            if (x < node->x_limit - margin) {
                node->cursor_x = x == 4 ? 6 : x + 1;
                Menu_PlayMoveSound();
                changed = 1;
            } else {
                link = node->linkedNext;
                if (link != 0) {
                    node->cursor_x = -1;
                    link->cursor_x = 0;
                    limit = link->y_limit - 1;
                    row = node->cursor_y - node->scroll_y + link->scroll_y;
                    if (row < limit)
                        limit = row;
                    link->cursor_y = limit;
                    g_MenuWidgetCurrentNode = link;
                    Menu_PlayMoveSound();
                    changed = 1;
                }
            }
        }
        held = MenuWidget_HeldShoulder();
        if (!(node->layout_flags & 2))
            changed |= 1;
        else
            changed |= held;
    } else if (buttons & 0x10000) {
        int from;
        int to;

        if (node->itemAction == 0)
            return changed;
        if (node->appearance.cursorTargetX >= 0) {
            from = MenuWidget_CursorCell(node);
            to = MenuWidget_TargetCell(node);
            if (from != to) {
                if (node->itemAction(node->selected_base, from, node->selected_base, to)) {
                    node->appearance.cursorTargetX = -1;
                    Menu_PlayConfirmSound();
                    changed = 1;
                } else {
                    Menu_PlayErrorSound();
                    changed = 1;
                }
            }
        }
        changed |= MenuWidget_SwapMarked(node, node->linkedPrevious);
        changed |= MenuWidget_SwapMarked(node, node->linkedNext);
    } else if (buttons & 0x40) {
        if (node->appearance.cursorTargetX >= 0) {
            node->cursor_x = node->appearance.cursorTargetX;
            node->cursor_y = node->target_y;
            node->appearance.cursorTargetX = -1;
            MenuWidget_ScrollToCursor(node);
            if (node->refreshItems != 0)
                node->refreshItems();
            Menu_PlayCancelSound();
            changed = 1;
        }
        changed |= MenuWidget_RestoreLinked(node, node->linkedPrevious);
        changed |= MenuWidget_RestoreLinked(node, node->linkedNext);
    } else if (node->popup_node != 0 && node == g_MenuWidgetCurrentNode) {
        if (buttons & 4) {
            row = node->cursor_y - node->visible_rows;
            if (row < 0)
                row = 0;
            node->cursor_y = row;
            Menu_StepListNavigate((MenuWidgetListNavigation *)node->popup_node, 0x1000);
            return changed;
        }
        if (buttons & 8) {
            node->cursor_y =
                node->y_limit - MenuWidget_EndMargin(node) < node->cursor_y + node->visible_rows
                    ? node->y_limit - MenuWidget_EndMargin(node)
                    : node->cursor_y + node->visible_rows;
            Menu_StepListNavigate((MenuWidgetListNavigation *)node->popup_node, 0x4000);
            return changed;
        }
    } else if (buttons & 0x20) {
        changed = 1;
    }
    return changed;
}
