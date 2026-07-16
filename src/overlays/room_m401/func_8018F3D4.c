#include "../room_lib/room_lib.h"

typedef struct RoomM401FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    void *asset;
} RoomM401FxMatrixState;

extern unsigned char D_801955C0;
extern unsigned char D_801955C1;
extern unsigned char D_801955C2;
extern unsigned char D_801955C4;
extern unsigned char D_801955C5;
extern unsigned char D_801955C6;
extern short D_801955C8;
extern short D_801955CA;
extern unsigned char D_801955D0;
extern unsigned char D_801955D1;
extern unsigned char D_801955D2;
extern unsigned char D_801955D4;
extern unsigned char D_801955D5;
extern unsigned char D_801955D6;
extern short D_801955D8;
extern short D_801955DA;

int *func_800C2B28(int arg0);

void func_8018F3D4(RoomEnt *ent, void *unused, RoomM401FxMatrixState *state) {
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
    D_801955D8 = -0x64;
    D_801955C4 = 4;
    D_801955C5 = 1;
    D_801955D4 = 0;
    D_801955D5 = 0;
    D_801955DA = height;
    D_801955D0 = color;
    D_801955D1 = color;
    D_801955D2 = color;
    D_801955D6 = 0;
    D_801955C8 = 0x32;
    D_801955CA = height;
    D_801955C0 = color;
    D_801955C1 = color;
    D_801955C2 = color;
    D_801955C6 = 0;
}
