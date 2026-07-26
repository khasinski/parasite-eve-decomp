typedef struct RoomM186DriftState {
    unsigned short x;
    short pad02;
    unsigned short z;
    short pad06;
    short dx;
    short pad0A;
    short dz;
    short pad0E;
    unsigned short phase10;
    short limit12;
    short phase14;
    unsigned short counter16;
} RoomM186DriftState;

void func_80190D3C(unsigned char *arg0, unsigned char *signal, RoomM186DriftState *state) {
    RoomM186DriftState *p = state;
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
