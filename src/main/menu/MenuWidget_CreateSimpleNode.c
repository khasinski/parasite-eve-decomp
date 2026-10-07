#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


void BoundsCheck_AssertStub(int arg0);
MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void);


void *MenuWidget_CreateSimpleNode(s32 arg0, void *arg1, void *arg2, s32 arg3) {
    s32 mode = arg0;
    register void *parent_arg asm("$19") = arg1;
    void *parent = arg2;
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
    node->target_x = 0;
    return node;
}

