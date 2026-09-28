#include "../room_lib/room_lib.h"

void func_801933F8(void *unused, unsigned char *state, RoomLibStepRecords *p) {
    unsigned int i;

    for (i = 0; i < 4; i++) {
        p->value[i][0] += p->increment[i][0];
        p->value[i][1] += p->increment[i][1];
        p->value[i][2] += p->increment[i][2];
        p->timer[i] -= 8;
        if ((short)p->timer[i] < 0) {
            p->timer[i] = 0;
        }
    }

    if (*(short *)(state + 2) >= 0x3D) {
        state[1] = 2;
    }
}
