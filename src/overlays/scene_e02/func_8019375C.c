#include "../room_lib/room_lib.h"

extern char *D_8009D254;
extern int D_800966EC[];

void D_80193868(void);

#define SCHED_BARRIER() __asm__ volatile("")

void func_8019375C(RoomEnt *o) {
    char *g = D_8009D254;
    if (RW8(g, 0xE) < 4) {
        int *sig;
        RW32(g, 0x98) &= 0xFFF3FFFF;
        sig = o->sub.signal;
        o->sub.cb = D_80193868;
        if (sig != 0) {
            *sig = 2;
        }
        if (RW16(o, 0x44) == -1) {
            RW16(o, 0x44) = FieldEng_VecToAngle(
                (int *)((char *)o->link + 0x28),
                (int *)(D_8009D254 + 0x28));
        }
        if (RW16(o, 0x48) == -1) {
            RW16(o, 0x48) = RWU16(o, 0x44) + 0x800;
        }
        {
            int *e = (int *)((char *)D_800966EC
                             + ((RWU16(o, 0x44) & 0xFFF) << 2));
            int hi = *(short *)((char *)e + 2);
            int lo;
            unsigned short copy;
            RW16(o, 0x1E) = 0;
            RW16(o, 0x1C) = hi;
            SCHED_BARRIER();
            lo = *e;
            copy = *(volatile unsigned short *)((char *)o + 0x1C);
            RW16(o, 0x22) = 0;
            RW16(o, 0x26) = 0;
            RW16(o, 0x2A) = 0;
            RW16(o, 0x20) = lo;
            SCHED_BARRIER();
            RW16(o, 0x24) = 0x1000;
            RW16(o, 0x2C) = copy;
            RW16(o, 0x28) = -RWU16(o, 0x20);
        }
    }
}
