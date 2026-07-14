#include "../room_lib/room_lib.h"

typedef struct RoomM087FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    void *asset;
} RoomM087FxMatrixState;

extern unsigned char D_80193628;
extern unsigned char D_80193629;
extern unsigned char D_8019362A;
extern unsigned char D_8019362C;
extern unsigned char D_8019362D;
extern unsigned char D_8019362E;
extern short D_80193630;
extern short D_80193632;
extern unsigned char D_80193638;
extern unsigned char D_80193639;
extern unsigned char D_8019363A;
extern unsigned char D_8019363C;
extern unsigned char D_8019363D;
extern unsigned char D_8019363E;
extern short D_80193640;
extern short D_80193642;

int *func_800C2B28(int arg0);

void func_8018F394(RoomEnt *ent, void *unused, RoomM087FxMatrixState *state) {
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
    D_80193640 = -0x64;
    D_8019362C = 4;
    D_8019362D = 1;
    D_8019363C = 0;
    D_8019363D = 0;
    D_80193642 = height;
    D_80193638 = color;
    D_80193639 = color;
    D_8019363A = color;
    D_8019363E = 0;
    D_80193630 = 0x32;
    D_80193632 = height;
    D_80193628 = color;
    D_80193629 = color;
    D_8019362A = color;
    D_8019362E = 0;
}
