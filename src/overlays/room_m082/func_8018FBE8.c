#include "../room_lib/room_lib.h"

extern void func_800C4E50(void *arg0);

void func_8018FBE8(void *arg0, void *arg1, char *obj) {
    int c80 = 0x80;
    int v = 0x10;

    RW16(obj, 0x120) = v;
    v = 0xC8;
    RW16(obj, 0x12) = c80;
    RW16(obj, 0x128) = c80;
    c80 = 0xFF;
    RW16(obj, 0x126) = v;
    v = 0x40;
    RW8(obj, 0x118) = v;
    v = 0x80;
    RW8(obj, 0x119) = v;
    v = 0x4B0;
    RW16(obj, 0x122) = v;
    v = (int)(obj + 0x14);
    RW16(obj, 0x10) = 0;
    RW8(obj, 0x11C) = c80;
    RW8(obj, 0x11D) = c80;
    RW8(obj, 0x11E) = c80;
    RW8(obj, 0x11A) = c80;
    RW16(obj, 0x124) = 0;
    RW32(obj, 0x114) = v;
    func_800C4E50(obj + 0x114);
}
