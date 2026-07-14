#include "../room_lib/room_lib.h"

extern void *D_8009D20C;

int func_8018F084(void *obj, int mode, int event, int arg3, int arg4) {
    register char *base asm("$8");
    register int arg4_reg asm("$9");
    register void *initial asm("$2");
    void *node;
    void *next;

    base = (char *)obj + 0xC;
    arg4_reg = arg4;
    switch (event) {
    case 0x19:
        if (mode == 1) {
            RW32(obj, 0x10) = arg3;
            *(int *)arg3 = mode;
        }
        break;
    case 0x200:
        initial = D_8009D20C;
        RW32(obj, 0x1C) = (int)initial;
        if (initial != 0) {
            while (1) {
                node = (void *)RW32(base, 0x10);
                if (RW8(node, 0xC) == arg3 && RW8(node, 0xD) == arg4_reg && (RW32(node, 0x98) & 0x10) == 0) {
                    break;
                }
                next = (void *)RW32((void *)RW32(base, 0x10), 4);
                RW32(base, 0x10) = (int)next;
                if (next != 0) {
                    continue;
                }
                break;
            }
        }
        break;
    }

    return 0;
}
