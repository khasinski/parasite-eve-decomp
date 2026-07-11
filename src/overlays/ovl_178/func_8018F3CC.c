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
    func_800C2B40(state);

    __asm__ volatile(
        "lw    $v0, 0x8(%0)\n"
        "nop\n"
        "sw    $v0, 0x0(%1)\n"
        "lw    $v0, 0x238($v0)\n"
        "nop\n"
        "lw    $v1, 0x0($v0)\n"
        "lw    $a1, 0x4($v0)\n"
        "lw    $a2, 0x8($v0)\n"
        "lw    $a3, 0xC($v0)\n"
        "sw    $v1, 0x4(%1)\n"
        "sw    $a1, 0x8(%1)\n"
        "sw    $a2, 0xC(%1)\n"
        "sw    $a3, 0x10(%1)\n"
        "lw    $v1, 0x10($v0)\n"
        "lw    $a1, 0x14($v0)\n"
        "lw    $a2, 0x18($v0)\n"
        "lw    $a3, 0x1C($v0)\n"
        "sw    $v1, 0x14(%1)\n"
        "sw    $a1, 0x18(%1)\n"
        "sw    $a2, 0x1C(%1)\n"
        "sw    $a3, 0x20(%1)\n"
        :
        : "r"(ent), "r"(state)
        : "v0", "v1", "a1", "a2", "a3", "memory");
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
