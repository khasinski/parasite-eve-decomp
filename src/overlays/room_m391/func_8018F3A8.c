#include "../room_lib/room_lib.h"

typedef struct RoomM391FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    void *asset;
} RoomM391FxMatrixState;

extern unsigned char D_80194548;
extern unsigned char D_80194549;
extern unsigned char D_8019454A;
extern unsigned char D_8019454C;
extern unsigned char D_8019454D;
extern unsigned char D_8019454E;
extern short D_80194550;
extern short D_80194552;
extern unsigned char D_80194558;
extern unsigned char D_80194559;
extern unsigned char D_8019455A;
extern unsigned char D_8019455C;
extern unsigned char D_8019455D;
extern unsigned char D_8019455E;
extern short D_80194560;
extern short D_80194562;

int *func_800C2B28(int arg0);

void func_8018F3A8(RoomEnt *ent, void *unused, RoomM391FxMatrixState *state) {
    RoomLibFxMatrixWords *matrix;
    int *h24Ptr;
    int h24;
    register int height asm("$4");
    register int color asm("$3");

    func_800C2B40(state);
    state->asset = func_8006DC18(0xA);
    state->link = ent->link;
    matrix = (RoomLibFxMatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->h26 = 0;
    state->h28 = 0;
    h24Ptr = func_800C2B28(6);
    height = 0x80;
    h24 = *h24Ptr;
    color = 0x80;
    state->h24 = h24;
    asm volatile("" : : : "memory");
    D_80194560 = -0x64;
    D_8019454C = 4;
    D_8019454D = 1;
    D_8019455C = 0;
    D_8019455D = 0;
    D_80194562 = height;
    D_80194558 = color;
    D_80194559 = color;
    D_8019455A = color;
    D_8019455E = 0;
    D_80194550 = 0x32;
    D_80194552 = height;
    D_80194548 = color;
    D_80194549 = color;
    D_8019454A = color;
    D_8019454E = 0;
}
