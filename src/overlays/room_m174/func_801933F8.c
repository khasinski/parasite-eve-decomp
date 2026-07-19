#include "../room_lib/room_lib.h"

typedef struct RoomM174StepRec {
    char pad00[0x30];
    unsigned short h30;
    unsigned short h32;
    unsigned short h34;
    char pad36[0x1A];
    unsigned short h50;
    unsigned short h52;
    unsigned short h54;
} RoomM174StepRec;

void func_801933F8(void *unused, unsigned char *state, RoomM174StepRec *p) {
    unsigned int i;
    char *base = (char *)p;

    for (i = 0; i < 4; i++) {
        char *rec = base + i * 8;
        char *timer = base + i * 2;

        RWU16(rec, 0x30) += RWU16(rec, 0x50);
        RWU16(rec, 0x32) += RWU16(rec, 0x52);
        RWU16(rec, 0x34) += RWU16(rec, 0x54);
        RWU16(timer, 0x78) -= 8;
        if ((short)RWU16(timer, 0x78) < 0) {
            RWU16(timer, 0x78) = 0;
        }
    }

    if (*(short *)(state + 2) >= 0x3D) {
        state[1] = 2;
    }
}
