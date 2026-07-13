#include "../room_lib/room_lib.h"

typedef struct RoomM393FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    short h2A;
    void *asset;
} RoomM393FxMatrixState;

extern short D_80194860;
extern short D_80194862;
extern unsigned char D_80194858;
extern unsigned char D_80194859;
extern unsigned char D_8019485A;
extern unsigned char D_8019485C;
extern unsigned char D_8019485D;
extern unsigned char D_8019485E;
extern unsigned char D_80194868;
extern unsigned char D_80194869;
extern unsigned char D_8019486A;
extern unsigned char D_8019486C;
extern unsigned char D_8019486D;
extern unsigned char D_8019486E;
extern short D_80194870;
extern short D_80194872;

int *func_800C2B28(int arg0);

void func_8018F3CC(RoomEnt *ent, void *unused, RoomM393FxMatrixState *state) {
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
    D_80194870 = -0x64;
    D_8019485C = 4;
    D_8019485D = 1;
    D_8019486C = 0;
    D_8019486D = 0;
    D_80194872 = height;
    D_80194868 = color;
    D_80194869 = color;
    D_8019486A = color;
    D_8019486E = 0;
    D_80194860 = 0x32;
    D_80194862 = height;
    D_80194858 = color;
    D_80194859 = color;
    D_8019485A = color;
    D_8019485E = 0;
}
