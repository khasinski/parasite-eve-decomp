/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef int s32;

void BoundsCheck_AssertStub(int arg0);
void *MenuWidget_LookupSimpleDescriptor(void);
void *MenuWidget_FindLastMode1WithCursorX(void);

extern void *g_MenuWidgetActiveListHead;
extern void *g_MenuWidgetFreeListHead;

#define W(base, off) (*(s32 *)((char *)(base) + (off)))

asm(".globl func_80062D2C");
asm("func_80062D2C = MenuWidget_CreateSimpleNode");

void *MenuWidget_CreateSimpleNode(s32 arg0, void *arg1, void *arg2, s32 arg3) {
    s32 mode = arg0;
    register void *parent_arg asm("$19") = arg1;
    void *parent = arg2;
    register s32 arg_flag asm("$21") = arg3;
    void *desc;
    void *node;
    void *next;
    void *old_head;

    desc = MenuWidget_LookupSimpleDescriptor();
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
            asm("addu %0,%0,%1" : "=r"(slot) : "r"(parent), "0"(slot));
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
    if (W(desc, 0) != 0) {
        W(node, 0x18) = W(desc, 0);
        W(node, 0x1C) = W(desc, 4);
    } else {
        register int x_base asm("$2");
        int width;
        int half_h;
        int flag;

        width = W(desc, 8);
        x_base = 0xA0;
        width >>= 1;
        x_base -= width;
        W(node, 0x18) = x_base;

        x_base = W(desc, 0xC);
        flag = W(desc, 4);
        half_h = x_base >> 1;
        if (flag != 0) {
            x_base = 0x50;
        } else {
            x_base = 0x78;
        }
        x_base -= half_h;
        W(node, 0x1C) = x_base;
    }

    W(node, 0x34) = W(desc, 8);
    W(node, 0x38) = W(desc, 0xC);
    W(node, 0x3C) = 0;
    W(node, 0x40) = 0;
    W(node, 0x44) = arg_flag;
    W(node, 0x48) = 0;
    W(node, 0x4C) = 0;
    return node;
}
