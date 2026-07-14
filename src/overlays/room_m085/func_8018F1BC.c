#include "../room_lib/room_lib.h"

typedef struct RoomM085FxMatrixState {
    RoomLink *link;
    RoomLibFxMatrixWords matrix;
    short h24;
    short h26;
    short h28;
    void *asset;
} RoomM085FxMatrixState;

extern unsigned char D_80190088;
extern unsigned char D_80190089;
extern unsigned char D_8019008A;
extern unsigned char D_8019008C;
extern unsigned char D_8019008D;
extern unsigned char D_8019008E;
extern short D_80190090;
extern short D_80190092;
extern unsigned char D_80190098;
extern unsigned char D_80190099;
extern unsigned char D_8019009A;
extern unsigned char D_8019009C;
extern unsigned char D_8019009D;
extern unsigned char D_8019009E;
extern short D_801900A0;
extern short D_801900A2;

int *func_800C2B28(int arg0);

void func_8018F1BC(RoomEnt *ent, void *unused, RoomM085FxMatrixState *state) {
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
    D_801900A0 = -0x64;
    D_8019008C = 4;
    D_8019008D = 1;
    D_8019009C = 0;
    D_8019009D = 0;
    D_801900A2 = height;
    D_80190098 = color;
    D_80190099 = color;
    D_8019009A = color;
    D_8019009E = 0;
    D_80190090 = 0x32;
    D_80190092 = height;
    D_80190088 = color;
    D_80190089 = color;
    D_8019008A = color;
    D_8019008E = 0;
}
