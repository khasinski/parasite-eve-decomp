#include "../room_lib/room_lib.h"

extern char *D_8009D20C;

int func_8018F084(char *obj, int arg1, int op, int arg3, int arg4) {
    char *node;
    char *base;
    int next;

    base = obj + 0xC;
    if (op == 0x19) {
        goto op19;
    }
    if (op == 0x200) {
        goto op200;
    }
    goto done;

op19:
    if (arg1 == 1) {
        *(int *)(obj + 0x10) = arg3;
        *(int *)arg3 = arg1;
    }
    goto done;

op200:
    next = (int)D_8009D20C;
    *(int *)(obj + 0x1C) = next;
    if (next != 0) {
        do {
            node = *(char **)(base + 0x10);
            if (*(unsigned char *)(node + 0xC) == arg3) {
                if (*(unsigned char *)(node + 0xD) == arg4) {
                    if ((*(int *)(node + 0x98) & 0x10) == 0) {
                        goto done;
                    }
                }
            }
            next = *(int *)(*(char **)(base + 0x10) + 4);
            *(int *)(base + 0x10) = next;
        } while (next != 0);
    }

done:
    return 0;
}
