#include "../room_lib/room_lib.h"

void func_8018FC24(RoomLink *link, struct RoomSub *sub, int scratch) {
    register char *s asm("$16");
    register RoomLink *l asm("$17");
    char *target;
    int angle;
    int done;
    register int limit asm("$2");
    register int value asm("$3");

    s = (char *)sub;
    l = link;
    if (RW16(s, 0xA2) > 0) {
        target = (char *)RW32(s, 0x84);
        if (target != 0) {
            RW32(s, 0x60) = RW32(target, 0x28);
            RW32(s, 0x68) = RW32(target, 0x30);
        }
        angle = FieldEng_VecToAngle((int *)(s + 0x60), &l->pos[0]);
        l->h3A = FieldEng_TurnToward(l->h3A, angle, RW16(s, 0xA2)) & 0xFFF;
        if (RW8(s, 0xA9) != 0) {
            limit = 0x7FFFFFF;
            value = RW32(s, 0x98);
            done = limit < value;
            if (done) {
                RW16(s, 0xA2) = 0;
                RW16(s, 0xA4) = 0;
                goto store_done;
            }
        }
    } else {
        func_800DFE94(&l->pos[0], (char *)l + 0x40, (char *)l + 0x38);
        done = FieldEng_TurnToward(l->h3A, l->h3A, RW16(s, 0xA4));
store_done:
        l->h3A = done & 0xFFF;
    }
}
