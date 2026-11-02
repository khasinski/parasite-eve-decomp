/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/bounds_check.h"
#include "pe1/draw_area.h"
#include "pe1/draw_state.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_scroll_cursor.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_gpu.h"
#include "pe1/text.h"
#include "pe1/textbox.h"

/* Contiguous widget construction, pool initialization, list drawing and layout. */

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
unsigned char *g_DrawPacketCursor;
unsigned char *g_DrawPacketArenaBase;
unsigned int g_DrawPrimColor;
unsigned int g_DrawColorShaded;
unsigned int *g_DrawOrderingTableEntry;
int g_DrawTextPosX;
int g_MenuListRowHeight;
int g_DrawBufferIndex;
int g_TextRenderMode;
int g_DrawGlyphAdvance;
int g_DrawTextBoxWidth;
int g_DrawTextDimmed;
int g_MenuInputActive;
int *g_TextCursorStackPtr;

void MenuWidget_InitPoolUnk(void) {
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
    g_MenuWidgetFreeListHead = &g_MenuWidgetNodePoolSentinel - 23;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}
#define NULL ((void *)0)

inline MenuWidgetNode *MenuWidget_AllocNode(MenuWidgetNode *arg0, MenuWidgetNode *arg1) {
    s32 var_a0;
    s32 var_v0;
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_v1;
    MenuWidgetNode *temp_next;
    MenuWidgetNode **var_a1;
    MenuWidgetNode *temp_s2;
    MenuWidgetNode *temp_s1;

    temp_s2 = arg0;
    temp_s1 = arg1;
    temp_s0 = g_MenuWidgetFreeListHead;
    if (temp_s0 == NULL) {
        Inv_BoundsCheckAssertStub(0xA);
    }
    var_a0 = 3;
    temp_next = temp_s0->next;
    temp_v1 = g_MenuWidgetActiveListHead;
    var_a1 = &temp_s0->children[1];
    g_MenuWidgetActiveListHead = temp_s0;
    temp_s0->parent = temp_s2;
    temp_s0->update = 0;
    temp_s0->draw = 0;
    g_MenuWidgetFreeListHead = temp_next;
    temp_s0->next = temp_v1;
    do {
        var_a1[2] = 0;
        var_a0 -= 1;
        var_a1--;
    } while (var_a0 >= 0);
    temp_s0->y = 0;
    temp_s0->x = 0;
    temp_s0->selected_base = 0;
    temp_s0->mode = 0;
    temp_s0->flags = 0;
    if (temp_s1 != NULL) {
        var_a0 = 0;
        while (var_a0 < 4 && temp_s1->children[var_a0] != 0) {
            var_a0 += 1;
        }
        var_v0 = var_a0 < 4;
        if (var_v0 != 0) {
            temp_s1->children[var_a0] = temp_s0;
        } else {
            Inv_BoundsCheckAssertStub(0xB, var_a1);
        }
    }
    return temp_s0;
}

MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetFreeListHead;
void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node) {
    MenuWidgetNode *current = g_MenuWidgetActiveListHead;
    MenuWidgetNode *previous = 0;
    int i;
    if (current) {
        while (current) {
            if (current == node) {
                break;
            }
            previous = current;
            current = current->next;
        }
        if (current) {
            if (previous) {
                previous->next = current->next;
            } else {
                g_MenuWidgetActiveListHead = current->next;
            }
            current->next = g_MenuWidgetFreeListHead;
            g_MenuWidgetFreeListHead = current;
            if (current->mode == 2) {
                MenuWidget_SaveColumnLayout(current);
            }
            for (i = 0; i < 4; i++) {
                if (current->children[i]) {
                    MenuWidget_DestroyNodeRecursive(current->children[i]);
                }
            }
            if (MenuWidget_GetCurrentNode() == current) {
                MenuWidget_SetCurrentNode(current->parent);
            }
            for (current = g_MenuWidgetActiveListHead; current; current = current->next) {
                if (current->parent == node) current->parent = 0;
                if (current->mode == 2) {
                    if (current->linkedPrevious == node) current->linkedPrevious = 0;
                    if (current->linkedNext == node) current->linkedNext = 0;
                }
                for (i = 0; i < 4; i++) {
                    if (current->children[i] == node) current->children[i] = 0;
                }
            }
        }
    }
}

#define NULL ((void *)0)

/* Draws a widget and its children, each relative to its parent's origin,
 * saving the text cursor on the cursor stack around it. */
void Draw_PushPrimToList(MenuWidgetNode *node) {
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    u32 temp_a0;
    u32 temp_a0_3;
    int *temp_a1;
    u32 temp_a1_2;
    MenuWidgetNode *child;
    int *q;
    s32 t0;
    u32 t1;
    void (*f)();
    s32 offsetX, offsetY;
    s32 cursorX;
    u32 cursorY;

    temp_a1 = g_TextCursorStackPtr;
    if (temp_a1 < g_TextCursorStackTop) {
        t0 = g_TextCursorX;
        t1 = g_TextCursorY;
        g_TextCursorStackPtr = temp_a1 + 2;
        temp_a1[0] = t0;
        temp_a1[1] = t1;
    } else {
        Inv_BoundsCheckAssertStub(2, temp_a1);
    }
    offsetX = node->x;
    cursorX = g_TextCursorX;
    f = node->draw;
    temp_a2 = cursorX + offsetX;
    offsetY = node->y;
    cursorY = g_TextCursorY;
    g_TextCursorX = temp_a2;
    temp_a1_2 = cursorY + offsetY;
    g_TextCursorY = temp_a1_2;
    if (f != NULL) {
        q = g_TextCursorStackPtr;
        if (q < g_TextCursorStackTop) {
            q[0] = temp_a2;
            q[1] = temp_a1_2;
            g_TextCursorStackPtr = q + 2;
        } else {
            Inv_BoundsCheckAssertStub(2, temp_a1_2, temp_a2);
        }
        node->draw(node);
        if (g_TextCursorStackBottom < g_TextCursorStackPtr) {
            temp_v0 = g_TextCursorStackPtr[-2];
            temp_a0 = g_TextCursorStackPtr[-1];
            g_TextCursorStackPtr -= 2;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
        } else {
            Inv_BoundsCheckAssertStub(3);
        }
    }
    var_s1 = 0;
    do {
        child = node->children[var_s1];
        if (child != NULL) {
            Draw_PushPrimToList(child);
        }
        var_s1 += 1;
    } while (var_s1 < 4);
    if (g_TextCursorStackBottom < g_TextCursorStackPtr) {
        temp_v0_2 = g_TextCursorStackPtr[-2];
        temp_a0_3 = g_TextCursorStackPtr[-1];
        g_TextCursorStackPtr -= 2;
        g_TextCursorX = temp_v0_2;
        g_TextCursorY = temp_a0_3;
        return;
    }
    Inv_BoundsCheckAssertStub(3);
}

MenuWidgetNode *g_MenuWidgetActiveListHead;

int MenuWidget_HasActiveNodes(void) {
    return g_MenuWidgetActiveListHead != 0;
}

MenuWidgetNode *MenuWidget_FindByFieldValue(int value) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;
    int i;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        i = 0;
        while (i < 4) {
            if ((int)node->children[i] == value) {
                break;
            }
            i++;
        }
        if (i < 4) {
            result = node;
            break;
        }
        node = node->next;
    }
    return result;
}

MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index) {
    return node->children[index];
}

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base) {
    MenuWidgetNode *node;

    node = g_MenuWidgetActiveListHead;
    while (node != 0) {
        if (node->mode == mode && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }
    return node;
}

/* The text cursor stack holds (x, y) word pairs. */
#define PUSH_CURSOR()                                                      \
    do {                                                                   \
        int *cursor = g_TextCursorStackPtr;                                \
        if (cursor < g_TextCursorStackTop) {                               \
            int x = g_TextCursorX;                                         \
            int y = g_TextCursorY;                                         \
            g_TextCursorStackPtr = cursor + 2;                             \
            cursor[0] = x;                                                 \
            cursor[1] = y;                                                 \
        } else {                                                           \
            Inv_BoundsCheckAssertStub(2);                                  \
        }                                                                  \
    } while (0)

#define PUSH_CURSOR2()                                     \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
        if (cursor < g_TextCursorStackTop) {               \
            int *next = cursor + 2;                        \
            int x = g_TextCursorX;                         \
            int y = g_TextCursorY;                         \
            cursor[0] = x;                                 \
            cursor[1] = y;                                 \
            g_TextCursorStackPtr = next;                   \
        } else {                                           \
            Inv_BoundsCheckAssertStub(2);                  \
        }                                                  \
    } while (0)

#define POP_CURSOR()                                       \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
                                                           \
        if (g_TextCursorStackBottom < cursor) {            \
            g_TextCursorX = cursor[-2];                    \
            g_TextCursorY = cursor[-1];                    \
            g_TextCursorStackPtr = cursor - 2;             \
        } else {                                           \
            Inv_BoundsCheckAssertStub(3);                  \
        }                                                  \
    } while (0)

void Draw_FlushPrimList(int textId) {
    u8 *text;
    u8 *cursor;
    int width;
    int glyph;
    u8 ch;
    int first;

    text = Text_LookupTable4(textId);

    PUSH_CURSOR();

    first = *text;
    cursor = text;
    width = 0;
    if (first != 0xFF) {
        do {
            ch = *cursor++;
            glyph = ch;
            if (g_TextRenderMode != 0) {
                glyph += g_TextRenderMode << 8;
                g_TextRenderMode = 0;
            }

            if (ch >= 0xFA) {
                g_TextRenderMode = ch - 0xFA;
                glyph = -1;
            }

            if (glyph >= 0) {
                if (glyph >= 0x100) {
                    glyph -= 0x13;
                }
                width += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 0xF) + g_DrawGlyphAdvance;
            }
        } while (*cursor != 0xFF);
        cursor = text;
    }

    {
        int adjusted = width + 4;
        int center = g_DrawTextBoxWidth - adjusted;
        int y = g_TextCursorY;
        int x = g_TextCursorX;
        center >>= 1;
        g_TextCursorY = y;
        g_TextCursorX = x + center;
    }

    if (cursor != 0) {
        PUSH_CURSOR2();

        {
            u8 code = *cursor;
            if ((code & 0xFF) != 0xFF) {
                for (;;) {
                    Draw_AllocTexturedQuad(code);
                    code = *++cursor;
                    if (code == 0xFF) break;
                }
            }
        }

        POP_CURSOR();
    }

    POP_CURSOR();
}

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

MenuWidgetNode *MenuWidget_CreateSimpleNode(s32 mode, MenuWidgetNode *parent_arg, MenuWidgetNode *parent, s32 arg_flag) {
    MenuWidgetSimpleDescriptor *desc;
    MenuWidgetNode *node;

    desc = MenuWidget_LookupSimpleDescriptor(mode);
    if (desc == 0) {
        Inv_BoundsCheckAssertStub(0xC);
    }

    node = MenuWidget_AllocNode(parent_arg, parent);

    if (node == 0) {
        Inv_BoundsCheckAssertStub(0xD);
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
        node->x = 0xA0 - (desc->width >> 1);
        node->y = (desc->y != 0 ? 0x50 : 0x78) - (desc->height >> 1);
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

void Draw_PushPrimToList(MenuWidgetNode *node);

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

MenuWidgetNode *MenuWidget_CreateNode(s32 mode, MenuWidgetNode *parent_arg, MenuWidgetNode *parent) {
    MenuWidgetGridDescriptor *desc;
    MenuWidgetNode *node;
    int tmp;

    desc = MenuWidget_LookupGridDescriptor(mode);
    if (desc == 0) {
        Inv_BoundsCheckAssertStub(0xE);
    }

    node = MenuWidget_AllocNode(parent_arg, parent);

    if (node == 0) {
        Inv_BoundsCheckAssertStub(0xF);
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
        node->update = Menu_StepScrollCursor;
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

void Draw_AllocColorTri(int, int, int);

typedef DrawTextCursorPair CursorPair;

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
        Inv_BoundsCheckAssertStub(1);
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
    if (g_DrawBufferIndex) y += 0xE0;
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
    g_TextCursorX = g_TextCursorX + dx;
    g_TextCursorY = g_TextCursorY + dy;
}

static inline void RowPopCursor(void)
{
    CursorPair *stack = (CursorPair *)g_TextCursorStack;
    if ((CursorPair *)g_TextCursorStackBottom < stack) {
        g_TextCursorStack = (int *)(stack - 1);
        g_TextCursorX = stack[-1].x;
        g_TextCursorY = stack[-1].y;
    } else {
        Inv_BoundsCheckAssertStub(3);
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

    if (stack < (CursorPair *)g_TextCursorStackTop) {
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = g_TextCursorX;
        stack->y = g_TextCursorY;
    } else {
        Inv_BoundsCheckAssertStub(2);
    }
    g_TextCursorX += 2;
    g_TextCursorY += 2;
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
        g_DrawTextDimmed = dimmed;
        if (draw_callback)
            draw_callback(node->grid_width * (node->scroll_y + row) + x);
        if ((VSync(-1) & 8) && x == node->appearance.cursorTargetX &&
            row + node->scroll_y == node->target_y) {
            g_TextCursorX -= 2;
            g_TextCursorY -= 2;
            Draw_AllocColorTri(node->draw_state, node->disabled, 0);
            g_TextCursorX += 2;
            g_TextCursorY += 2;
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
    Draw_LinkDrawArea(g_DrawOrderingTableEntry, packet);
}

static inline void SetCursor(u32 x, u32 y)
{
    g_TextCursorX = x;
    g_TextCursorY = y;
}

static inline void MoveCursor(u32 dx, u32 dy)
{
    SetCursor(g_TextCursorX + dx, g_TextCursorY + dy);
}

static inline void PushCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        /* Capture both coordinates before writing into the shared stack. */
        int x = g_TextCursorX, y = g_TextCursorY;
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = x;
        stack->y = y;
    } else {
        Inv_BoundsCheckAssertStub(2);
    }
}

static inline void PopCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if ((DrawTextCursorPair *)g_TextCursorStackBottom < stack) {
        g_TextCursorStack = (int *)(stack - 1);
        SetCursor(stack[-1].x, stack[-1].y);
    } else {
        Inv_BoundsCheckAssertStub(3);
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
    g_DrawTextPosX = node->draw_state;
    g_MenuListRowHeight = node->disabled;
    rect.x = 0; rect.y = 0; rect.w = 320; rect.h = 224;
    if (g_DrawBufferIndex) rect.y = 224;
    packet = AllocRect(&rect);
    Draw_LinkDrawArea(g_DrawOrderingTableEntry, packet);
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
        g_TextCursorX += Product(node->draw_state, node->cursor_x);
        g_TextCursorY += Product(node->disabled, Difference(node->cursor_y, node->scroll_y));
        highlight = 0;
        if (!(node->layout_flags & 0x80) && g_MenuWidgetCurrentNode == node) {
            int blink = g_MenuInputActive && (Draw_RemapStatusFlags() & 0x20);
            if (blink) highlight = 1;
        }
        Draw_AllocColorTriGradient(node->draw_state, node->disabled, highlight,
                                   g_MenuWidgetCurrentNode == node);
        PopCursor();
    }
}

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
    if (g_MenuInputActive)
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
        g_MenuWidgetCurrentNode = other;
        done = 1;
    }
    return done;
}

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags);

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
    if (g_MenuInputActive)
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

void Draw_SetPrimCallback(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *temp_a1_2;
    s32 temp_a2;
    s32 temp_v1;
    s32 t58b;
    s32 t48;
    s32 t5C;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a1;
    s32 var_a2;
    MenuWidgetNode *temp_a1;

    temp_v1 = arg0->grid_width;
    arg0->y_limit = (((arg1 + temp_v1) - 1) / temp_v1);
    var_a2 = 0;
    if (arg0->grid_width == 2) {
        var_a2 = arg1 & 1;
    }
    t48 = arg0->cursor_y;
    arg0->has_scroll = var_a2;
    temp_v1_2 = arg0->y_limit;
    if (t48 >= temp_v1_2) {
        arg0->cursor_y = temp_v1_2 - 1;
    }
    if ((arg0->has_scroll != 0) && (arg0->cursor_x == 1) &&
        (arg0->cursor_y == (arg0->y_limit - 1))) {
        arg0->cursor_x = 0;
    }
    var_a1 = arg0->visible_rows_mirror;
    temp_v1_3 = arg0->y_limit;
    temp_a2 = arg0->visible_rows;
    if (temp_v1_3 < var_a1) {
        var_a1 = temp_v1_3;
        temp_v1_3 = *(volatile s32 *)&arg0->y_limit;
    }
    temp_v1_4 = temp_v1_3 - var_a1;
    arg0->visible_rows = var_a1;
    if (temp_v1_4 < arg0->scroll_y) {
        arg0->scroll_y = temp_v1_4;
    }
    temp_a1 = arg0->parent;
    if ((temp_a1 != 0) && (temp_a1->mode == 1)) {
        temp_a1->visible_rows += arg0->disabled * (arg0->visible_rows - temp_a2);
    }
    temp_a1_2 = arg0->popup_node;
    if (temp_a1_2 != 0) {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror >= t58b) {
            MenuWidget_DestroyPopupNode(temp_a1_2);
        }
    } else {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror < t58b) {
            Draw_SwapPrimBuffers(arg0);
        }
    }
}

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

/* Saved cursor/scroll positions of menu widgets (one 4-byte slot per
 * aux_index): reset, mode, save and restore. Menu_InitStateBuffer (historical
 * name) is the unconditional reset; the eight slots set to -1 start with no
 * saved column. */

void Menu_InitStateBuffer(void) {
    bzero(g_MenuWidgetColumnLayoutTable, 0x120);
    g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
}

void MenuWidget_SetColumnLayoutMode(int arg0) {
    g_MenuWidgetColumnLayoutMode = arg0;
    if (arg0 == 0) {
        bzero(g_MenuWidgetColumnLayoutTable, 0x120);
        g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
    }
}

int MenuWidget_GetColumnLayoutMode(void) {
    return g_MenuWidgetColumnLayoutMode;
}

void MenuWidget_SaveColumnLayout(MenuWidgetNode *node) {
    int index;
    MenuWidgetSavedLayout *slot;

    if (g_MenuWidgetColumnLayoutMode == 0 && ((node->layout_flags & 0x20) == 0)) {
        return;
    }

    index = node->aux_index;
    if ((unsigned int)index < 0x48) {
        slot = &g_MenuWidgetColumnLayoutTable[index];
        slot->cursorX = node->cursor_x;
        slot->cursorY = node->cursor_y;
        slot->scrollY = node->scroll_y;
    }
}

void MenuWidget_ApplyColumnLayout(MenuWidgetNode *node) {
    MenuWidgetSavedLayout *entry;
    int flag;

    if (node == 0) {
        return;
    }
    if (node->aux_index < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[node->aux_index];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_SetColumnLayout(MenuWidgetNode *node, int arg1) {
    MenuWidgetSavedLayout *entry;
    int flag;

    node->aux_index = arg1;
    if (node == 0) {
        return;
    }
    if (arg1 < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[arg1];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_ClearColumnLayout(MenuWidgetNode *node) {

    node->aux_index = -1;
    node->cursor_x = -1;
}

void MenuWidget_DrawCenteredText(u8 *text) {
    Draw_PrintCenteredTextInWidth(text, g_DrawTextPosX);
}

void MenuWidget_DrawCenteredTableText(int text_id) {
    Draw_PrintCenteredTextInWidth(Text_LookupTable4(text_id), g_DrawTextPosX);
}

void Draw_AllocColorTri(int width, int height, int arg2);

void Draw_DropShadow(void) {
    g_DrawPrimColor = 0xFF4000;
    g_DrawColorShaded = 0x7F2000;
    g_DrawSpriteX -= 2;
    g_DrawSpriteY -= 2;
    Draw_AllocColorTri(g_DrawTextPosX, g_MenuListRowHeight, 0);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    g_DrawSpriteX += 2;
    g_DrawSpriteY += 2;
}

MenuWidgetNode *g_MenuWidgetFreeListHead;

static inline MenuWidgetNode *Allocate(MenuWidgetNode *parent,
                                       MenuWidgetNode *owner)
{
    MenuWidgetNode *node = g_MenuWidgetFreeListHead;
    MenuWidgetNode *next;
    MenuWidgetNode *old;
    int i;

    if (!node)
        Inv_BoundsCheckAssertStub(10);
    next = node->next;
    old = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    node->parent = parent;
    node->update = 0;
    node->draw = 0;
    g_MenuWidgetFreeListHead = next;
    node->next = old;
    for (i = 3; i >= 0; i--)
        node->children[i] = 0;
    node->y = 0;
    node->x = 0;
    node->selected_base = 0;
    node->mode = 0;
    node->flags = 0;
    if (owner) {
        for (i = 0; i < 4; i++) {
            if (!owner->children[i])
                break;
        }
        if (i < 4)
            owner->children[i] = node;
        else
            Inv_BoundsCheckAssertStub(11);
    }
    return node;
}

/* Historical symbol name: constructs a mode-3 list-navigation widget. */
void Draw_SwapPrimBuffers(MenuWidgetNode *list)
{
    MenuWidgetNode *owner = FindOwner(list);
    MenuWidgetNode *node = Allocate(list->parent, owner);
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)node;

    if (!node)
        Inv_BoundsCheckAssertStub(16);
    node->mode = 3;
    node->draw = Draw_FlushFrontBuffer;
    node->update = Menu_StepListNavigate;
    navigation->list = list;
    navigation->flags = list->layout_flags;
    list->popup_node = node;
}

void MenuWidget_DestroyPopupNode(MenuWidgetNode *node) {
    MenuWidgetNode *owner = ((MenuWidgetListNavigation *)node)->list;

    owner->popup_node = 0;
    MenuWidget_DestroyNodeRecursive(node);
}

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

static inline void Offset(int x, int y) {
    g_DrawSpriteX += x;
    g_DrawSpriteY += y;
}

void Draw_FlushFrontBuffer(MenuWidgetListNavigation *node) {
    MenuWidgetNode *list = node->list;
    if (list) {
        MenuWidgetNode *owner = FindOwner((MenuWidgetNode *)node);
        DrawTextCursorPair *stack;
        g_DrawColorSelect = owner->mode == 1 && owner->draw_state != 0;
        stack = (DrawTextCursorPair *)g_TextCursorStack;
        if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
            int x = g_DrawSpriteX;
            int y = g_DrawSpriteY;
            g_TextCursorStack = (int *)(stack + 1);
            stack->x = x;
            stack->y = y;
        } else {
            Inv_BoundsCheckAssertStub(2);
        }
        Offset(list->draw_state * list->grid_width + 2,
               node->visibleRows + 2);
        if (g_MenuWidgetCurrentNode == (MenuWidgetNode *)node && (VSync(-1) & 8))
            Draw_AllocColorTri(8, node->drawState, 0);
        Draw_AllocColorGradient(8, node->drawState, 0, 1);
        if (list->scroll_y) {
            Offset(0, -6);
            Draw_AllocSprite(0x4a);
            Offset(0, 6);
        }
        if (list->scroll_y < list->y_limit - list->visible_rows) {
            Offset(0, node->drawState + 2);
            Draw_AllocSprite(0x4b);
        }
        PopCursor();
    }
}

static inline int Move(MenuWidgetNode *node, int delta) {
    int changed = 0;

    if (!node->scroll_adjust) {
        int old = node->scroll_y;
        node->scroll_y += delta;
        if (node->scroll_y < 0) {
            node->scroll_y = 0;
        } else if (node->scroll_y > node->y_limit - node->visible_rows) {
            node->scroll_y = node->y_limit - node->visible_rows;
        }
        changed = old != node->scroll_y;
        if (changed) {
            int speed = node->disabled;
            node->scroll_adjust = (delta > 0 ? speed : -speed) / 2;
        }
    }
    return changed;
}

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags) {
    MenuWidgetNode *list = node->list;
    int handled = 0;

    if (flags & 0x1004) {
        if (Move(list, -list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    } else if (flags & 0x4008) {
        if (Move(list, list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    }
    return handled || !(node->flags & 0x40);
}

#define NULL ((void *)0)
void MenuWidget_EaseNodePosition(MenuWidgetNode *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    MenuWidgetNode *temp_a1;
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)arg0;

    temp_a1 = navigation->list;
    if (arg0 != NULL) {
        temp_v1 = temp_a1->visible_rows;
        temp_a2 = temp_a1->y_limit;
        if (temp_v1 < temp_a2) {
            navigation->drawState = ((temp_a1->disabled * temp_v1 * temp_v1) / temp_a2);
            temp_v1_2 = temp_a1->disabled;
            navigation->visibleRows = ((temp_v1_2 * temp_a1->visible_rows * ((temp_v1_2 * temp_a1->scroll_y) - temp_a1->scroll_adjust)) / (temp_a1->y_limit * temp_v1_2));
        }
    }
}
