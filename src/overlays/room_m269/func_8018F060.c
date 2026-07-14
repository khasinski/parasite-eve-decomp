#include "../room_lib/room_lib.h"

extern int func_8018F160();

int func_8018F060(void *obj) {
    RW32(obj, 0xC) = (int)&func_8018F160;
    RW8(obj, 3) = 0xFF;
    RW8(obj, 0x1A) = 0;
    RW8(obj, 0x28) = 0;
    return 0;
}
