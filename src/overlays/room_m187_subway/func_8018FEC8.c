#include "pe1/room_fx.h"

void func_8018FEC8(unsigned char *arg0, unsigned char *signal, RoomFxDriftState *state) {
    RoomFxDriftState *p = state;
    unsigned short counter = state->counter16;

    state->counter16 = counter + 1;
    if ((short)counter >= 16) {
        state->counter16 = 15;
    }

    if (state->limit12 >= 9) {
        state->limit12 -= 8;
    }
    if (state->phase14 >= 21) {
        state->phase14 -= 20;
    }

    p->phase10 -= 0x64;
    p->x += (p->dx * p->phase14) >> 16;
    p->z += (p->dz * p->phase14) >> 16;

    if (*(short *)(signal + 2) >= 21) {
        signal[1] = 2;
    }
}
