#include "../room_lib/room_lib.h"

int func_80193250(char *o) {
    register int mask asm("$5");
    register char *link asm("$3");
    register char *p asm("$2");
    register int four asm("$6");

    mask = 0xFFFEFFFF;
    link = (char *)RVW32(o, 0x8);
    four = 4;
    o[0] = four;
    o[3] = 0;
    RW32(link, 0x98) &= mask;
    link = (char *)RVW32(o, 0x8);
    RWU16(link, 0x250) &= 0xFBFF;
    p = (char *)RVW32(o, 0x8);
    p = (char *)RW32(p, 0x18C);
    if (p != 0) {
        p = (char *)RW32(p, 0x0);
        if (p != 0) {
            p = (char *)RW32(p, 0x18);
            p[0] = four;
        }
    }
    return 0;
}
