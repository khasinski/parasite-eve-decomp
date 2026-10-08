/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/menu_widget.h"
#include "pe1/draw_state.h"
#include "pe1/menu_scroll_cursor.h"

/* Contiguous widget construction, pool initialization, list drawing and layout. */
void BoundsCheck_AssertStub(int arg0);
MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void);


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
        register s32 *ptr asm("$5");

        i = 3;
        ptr = (s32 *)((char *)node + 0xC);
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
void BoundsCheck_AssertStub(int arg0);
void MenuWidget_ApplyColumnLayout(void *node);


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
        register s32 *ptr asm("$5");

        i = 3;
        ptr = (s32 *)((char *)node + 0xC);
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
