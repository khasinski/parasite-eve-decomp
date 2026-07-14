#include "../room_lib/room_lib.h"

extern unsigned char D_8019A8E0[];

int func_801925B0(void *obj, int mode, int event, int arg3, int index) {
    switch (event) {
    case 0x19:
        if (mode == 1) {
            RW32(obj, 0x10) = arg3;
            *(int *)arg3 = mode;
        }
        break;
    case 0x1D:
        RW16(obj, 0x14) = arg3;
        RW8(obj, 0x4C) = D_8019A8E0[index];
        break;
    }

    return 0;
}
