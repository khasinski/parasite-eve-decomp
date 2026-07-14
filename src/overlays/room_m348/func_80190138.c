#include "../room_lib/room_lib.h"

void func_80190138(void *unused, void *unused2, char *p) {
    char *root = (char *)func_800C2B50();
    register unsigned int i asm("$6") = 0;
    register int one asm("$9") = 1;
    register int x80 asm("$8") = 0x80;
    register char *a0 asm("$4");
    register char *a1 asm("$5") = p;
    register char *a3 asm("$7") = a1;
    register int y asm("$2");

    RW16(p, 0xEA) = 0x80;
    RW32(p, 0xE0) = 0xFFEC0000;
    y = RW32(root, 0x58);
    a0 = a1;
    RW32(p, 0xE4) = y << 8;

    do {
        RW16(a0, 0x88) = 0;
        RW32(a1, 0xA8) = 0;
        a1 += 4;
        RW32(a3, 0xC) = RW16(p, 2) << 16;
        a3 += 0x10;
        RW8(p + i, 0xC8) = one;
        RW16(a0, 0xD0) = x80;
        i++;
        a0 += 2;
    } while (i < 8);
}
