#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"

void BoundsCheck_AssertStub(int arg0);
void *MenuWidget_FindLastMode1WithCursorX(void);

#define W(base, off) (*(s32 *)((char *)(base) + (off)))
#define DESCRIPTOR_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(MenuWidgetSimpleDescriptor, member)))

void *MenuWidget_CreateSimpleNode(s32 arg0, void *arg1, void *arg2, s32 arg3) {
    s32 mode = arg0;
    register void *parent_arg asm("$19") = arg1;
    void *parent = arg2;
    register s32 arg_flag asm("$21") = arg3;
    MenuWidgetSimpleDescriptor *desc;
    void *node;
    void *next;
    void *old_head;

    desc = MenuWidget_LookupSimpleDescriptor(mode);
    if (desc == 0) {
        BoundsCheck_AssertStub(0xC);
    }

    node = g_MenuWidgetFreeListHead;
    if (node == 0) {
        BoundsCheck_AssertStub(0xA);
    }

    next = (void *)W(node, 0);
    old_head = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    W(node, 4) = (s32)parent_arg;
    W(node, 0x2C) = 0;
    W(node, 0x30) = 0;
    g_MenuWidgetFreeListHead = next;
    W(node, 0) = (s32)old_head;

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

    W(node, 0x1C) = 0;
    W(node, 0x18) = 0;
    W(node, 0x24) = 0;
    W(node, 0x20) = 0;
    W(node, 0x28) = 0;

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
        void *found;

        found = MenuWidget_FindLastMode1WithCursorX();
        if (found != 0) {
            void *head;
            void *found_next;
            void *node_next;

            head = g_MenuWidgetActiveListHead;
            found_next = (void *)W(found, 0);
            node_next = (void *)W(head, 0);
            W(head, 0) = (s32)found_next;
            W(found, 0) = (s32)head;
            g_MenuWidgetActiveListHead = node_next;
        }
    }

    W(node, 0x20) = 1;
    W(node, 0x24) = mode;
    if (DESCRIPTOR_FIELD(desc, s32, x) != 0) {
        W(node, 0x18) = DESCRIPTOR_FIELD(desc, s32, x);
        W(node, 0x1C) = DESCRIPTOR_FIELD(desc, s32, y);
    } else {
        register int x_base asm("$2");
        int width;
        int half_h;
        int flag;

        width = DESCRIPTOR_FIELD(desc, s32, width);
        x_base = 0xA0;
        width >>= 1;
        x_base -= width;
        W(node, 0x18) = x_base;

        x_base = DESCRIPTOR_FIELD(desc, s32, height);
        flag = DESCRIPTOR_FIELD(desc, s32, y);
        half_h = x_base >> 1;
        if (flag != 0) {
            x_base = 0x50;
        } else {
            x_base = 0x78;
        }
        x_base -= half_h;
        W(node, 0x1C) = x_base;
    }

    W(node, 0x34) = DESCRIPTOR_FIELD(desc, s32, width);
    W(node, 0x38) = DESCRIPTOR_FIELD(desc, s32, height);
    W(node, 0x3C) = 0;
    W(node, 0x40) = 0;
    W(node, 0x44) = arg_flag;
    W(node, 0x48) = 0;
    W(node, 0x4C) = 0;
    return node;
}

#undef DESCRIPTOR_FIELD
