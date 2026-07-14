#include "../room_lib/room_lib.h"

typedef struct RoomM399FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    void *asset;
} RoomM399FxMatrixState;

extern unsigned char D_80194390;
extern unsigned char D_80194391;
extern unsigned char D_80194392;
extern unsigned char D_80194394;
extern unsigned char D_80194395;
extern unsigned char D_80194396;
extern short D_80194398;
extern short D_8019439A;
extern unsigned char D_801943A0;
extern unsigned char D_801943A1;
extern unsigned char D_801943A2;
extern unsigned char D_801943A4;
extern unsigned char D_801943A5;
extern unsigned char D_801943A6;
extern short D_801943A8;
extern short D_801943AA;

int *func_800C2B28(int arg0);

void func_8018F39C(RoomEnt *ent, void *unused, RoomM399FxMatrixState *state) {
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
    D_801943A8 = -0x64;
    D_80194394 = 4;
    D_80194395 = 1;
    D_801943A4 = 0;
    D_801943A5 = 0;
    D_801943AA = height;
    D_801943A0 = color;
    D_801943A1 = color;
    D_801943A2 = color;
    D_801943A6 = 0;
    D_80194398 = 0x32;
    D_8019439A = height;
    D_80194390 = color;
    D_80194391 = color;
    D_80194392 = color;
    D_80194396 = 0;
}
