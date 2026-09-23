#include "common.h"
#include "../room_lib/room_lib.h"

typedef struct SceneE08MatrixWords {
    int words[8];
} SceneE08MatrixWords;

typedef struct SceneE08State {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
    u16 timer;
    u16 counter;
    u16 flag;
} SceneE08State;

typedef struct SceneE08Color {
    u8 r, g, b, pad3;
    u8 texture, mode, extra, pad7;
    s16 offset, intensity;
} SceneE08Color;

extern SceneE08Color D_801994B8, D_801995A8, D_801995B8, D_80199690;
extern void func_800C2B40(void *);
extern void *func_8006DC18(int);

void func_80190060(RoomEnt *ent, void *unused, SceneE08State *state) {
    SceneE08MatrixWords *matrix;

    func_800C2B40(state);
    state->link = ent->link;
    matrix = (SceneE08MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->asset = func_8006DC18(0x2E);
    state->timer = 30;
    state->counter = 0;
    state->flag = 0;

    D_801994B8.texture = 0x46;
    D_801994B8.mode = 3;
    D_801994B8.offset = -400;
    D_801994B8.intensity = 128;
    D_801994B8.r = 240;
    D_801994B8.g = 64;
    D_801994B8.b = 64;
    D_801994B8.extra = 0;

    D_801995A8.texture = 0xA4;
    D_801995A8.mode = 10;
    D_801995A8.offset = -30;
    D_801995A8.intensity = 128;
    D_801995A8.r = 160;
    D_801995A8.g = 128;
    D_801995A8.b = 240;
    D_801995A8.extra = 0;

    D_801995B8.texture = 0x44;
    D_801995B8.mode = 2;
    D_801995B8.offset = -30;
    D_801995B8.intensity = 128;
    D_801995B8.r = 160;
    D_801995B8.g = 128;
    D_801995B8.b = 240;
    D_801995B8.extra = 0;

    D_80199690.texture = 0x4C;
    D_80199690.mode = 5;
    D_80199690.offset = -30;
    D_80199690.intensity = 128;
    D_80199690.r = 160;
    D_80199690.g = 128;
    D_80199690.b = 240;
    D_80199690.extra = 0;
}
