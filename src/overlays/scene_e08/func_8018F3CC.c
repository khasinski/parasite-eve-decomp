#include "../room_lib/room_lib.h"

typedef int s32;

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} Overlay178MatrixWords;

typedef struct {
    RoomLink *link;
    Overlay178MatrixWords matrix;
    void *asset;
} Overlay178InitState;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char pad3;
    unsigned char t4;
    unsigned char t5;
    unsigned char t6;
    unsigned char pad7;
    short h8;
    short hA;
} Overlay178FxColor;

extern Overlay178FxColor D_801994A8;
extern Overlay178FxColor D_80199670;

void func_800C2B40(void *arg0);
void *func_8006DC18(s32 type);

void func_8018F3CC(RoomEnt *ent, void *arg1, Overlay178InitState *state) {
    Overlay178MatrixWords *matrix;

    func_800C2B40(state);

    state->link = ent->link;
    matrix = (Overlay178MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->asset = func_8006DC18(0x2E);

    D_80199670.t4 = 0x46;
    D_80199670.t5 = 3;
    D_80199670.h8 = -0x1E;
    D_80199670.hA = 0x80;
    D_80199670.r = 0xA0;
    D_80199670.g = 0x80;
    D_80199670.b = 0xF0;
    D_80199670.t6 = 0;
    D_801994A8.t4 = 0x46;
    D_801994A8.t5 = 3;
    D_801994A8.h8 = -0x190;
    D_801994A8.hA = 0x80;
    D_801994A8.r = 0xA0;
    D_801994A8.g = 0x80;
    D_801994A8.b = 0xF0;
    D_801994A8.t6 = 0;
}
