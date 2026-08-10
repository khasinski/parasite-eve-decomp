#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void BoundsCheck_AssertStub(int arg0);
void Menu_StepScrollCursor(void);
void Draw_SwapPrimBuffers(void *node);
void MenuWidget_ApplyColumnLayout(void *node);

#define W(base, off) (*(s32 *)((char *)(base) + (off)))
#define DESCRIPTOR_FIELD(base, type, member) \
    (*(type *)((char *)(base) + PE1_OFFSETOF(MenuWidgetGridDescriptor, member)))

void *MenuWidget_CreateNode(s32 arg0, void *arg1, void *arg2) {
    s32 mode = arg0;
    register void *parent_arg asm("$20") = arg1;
    void *parent = arg2;
    MenuWidgetGridDescriptor *desc;
    void *node;
    void *next;
    void *old_head;
    int tmp;

    desc = MenuWidget_LookupGridDescriptor(mode);
    if (desc == 0) {
        BoundsCheck_AssertStub(0xE);
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
        BoundsCheck_AssertStub(0xF);
    }

    W(node, 0x20) = 2;
    W(node, 0x70) = mode;
    W(node, 0x24) = mode;
    {
        int t;
        int u;

        t = DESCRIPTOR_FIELD(desc, s32, x);
        W(node, 0x18) = t;
        u = DESCRIPTOR_FIELD(desc, s32, y);
        t = (s32)Menu_StepScrollCursor;
        W(node, 0x2C) = t;
        W(node, 0x1C) = u;
        t = DESCRIPTOR_FIELD(desc, s32, gridWidth);
        W(node, 0x54) = t;
        W(node, 0x34) = t;
        t = DESCRIPTOR_FIELD(desc, s32, visibleRows);
        W(node, 0x6C) = t;
        W(node, 0x38) = t;
        t = DESCRIPTOR_FIELD(desc, s32, yLimit);
        W(node, 0x58) = t;
        t = DESCRIPTOR_FIELD(desc, s32, initialDrawState);
        W(node, 0x3C) = t;
        u = DESCRIPTOR_FIELD(desc, s32, initialDisabled);
        tmp = u;
    }
    W(node, 0x48) = 0;
    W(node, 0x44) = 0;
    W(node, 0x50) = -1;
    W(node, 0x4C) = -1;
    W(node, 0x5C) = 0;
    W(node, 0x60) = 0;
    W(node, 0x40) = tmp;
    tmp = DESCRIPTOR_FIELD(desc, s32, layoutFlags);
    W(node, 0x7C) = 0;
    W(node, 0x78) = 0;
    W(node, 0x84) = 0;
    W(node, 0x88) = 0;
    W(node, 0x68) = 0;
    W(node, 0x64) = tmp;
    W(node, 0x8C) = 0;
    W(node, 0x80) = 0;
    if (W(node, 0x38) < W(node, 0x58)) {
        Draw_SwapPrimBuffers(node);
    }
    MenuWidget_ApplyColumnLayout(node);
    return node;
}

#undef DESCRIPTOR_FIELD
