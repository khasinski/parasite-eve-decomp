/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"
#include "pe1/menu_scroll_cursor.h"

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


void *MenuWidget_CreateNode(s32 arg0, void *arg1, void *arg2) {
    s32 mode = arg0;
    register void *parent_arg asm("$20") = arg1;
    void *parent = arg2;
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
        register s32 *ptr asm("$3");

        i = 0;
        ptr = (s32 *)parent;
        while (i < 4 && ptr[2] != 0) {
            i++;
            ptr++;
        }
        if (i < 4) {
            s32 *slot;

            slot = (s32 *)(i << 2);
            slot = (s32 *)((int)slot + (int)parent);

            slot[2] = (s32)node;
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
    node->target_x = -1;
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

