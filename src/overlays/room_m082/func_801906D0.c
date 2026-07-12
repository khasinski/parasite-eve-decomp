#include "../room_lib/room_lib.h"

typedef struct RoomLocalState {
    unsigned char b0;
    unsigned char state;
    short tick;
} RoomLocalState;

typedef struct RoomLocalSystem {
    char pad0[0x180];
    short phase;
    short count;
} RoomLocalSystem;

extern int func_80071A54(void);

#define U16(base, offset) (*(unsigned short *)((char *)(base) + (offset)))

void func_801906D0(void *arg0, RoomLocalState *st, RoomLocalSystem *sys) {
    register RoomLocalState *stp asm("s3") = st;
    register RoomLocalSystem *sysp asm("s2") = sys;
    register char *slot asm("s0");
    register char *particle asm("a2");
    volatile char pad[0x10];
    int i;
    int value;

    asm("" : : "r"(stp), "r"(sysp));
    i = 0;
    if (sysp->count != 0) {
        slot = (char *)sysp;
        do {
            value = func_80071A54();
            U16(slot, 0x160) = value % 8;
            value = func_80071A54();
            U16(slot, 0x162) = value % 8;
            slot += 4;
            i++;
        } while ((unsigned)i < sysp->count);
    }

    if (stp->tick >= 0x1F) {
        if (sysp->phase >= 5) {
            sysp->phase -= 4;
        } else {
            sysp->phase = 0;
        }

        i = 0;
        if (sysp->count != 0) {
            particle = (char *)sysp;
            do {
                i++;
                U16(particle, 0x40) += U16(particle, 0xC0);
                U16(particle, 0x42) += U16(particle, 0xC2);
                U16(particle, 0x44) += U16(particle, 0xC4);
                U16(particle, 0x48) += U16(particle, 0xC8);
                U16(particle, 0x4A) += U16(particle, 0xCA);
                U16(particle, 0x4C) += U16(particle, 0xCC);
                particle += 0x10;
            } while ((unsigned)i < sysp->count);
        }
    }

    if ((unsigned short)(stp->tick - 1) < 0x1FU) {
        sysp->phase += 4;
    }

    if (stp->tick == 0x41) {
        stp->state = 2;
    }
}
