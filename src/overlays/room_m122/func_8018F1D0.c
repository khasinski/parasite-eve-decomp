#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern void *func_8006E498(void *arg0, int arg1);
extern unsigned char D_801910A8;
extern unsigned char D_801910A9;
extern unsigned char D_801910AA;
extern unsigned char D_801910AC;
extern unsigned char D_801910AD;
extern unsigned char D_801910AE;
extern short D_801910B0;
extern short D_801910B2;
extern void *D_801910B4;

void func_8018F1D0(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
    RoomLibFxMatrixWords *matrix;
    void *asset;
    void *bank;
    int magic;

    func_800C2B40(state);
    state->link = ent->link;
    matrix = (RoomLibFxMatrixWords *)state->link->p238;
    state->matrix = *matrix;
    RW16(state, 0x2A) = 0;
    RW16(state, 0x2C) = 0;
    RW16(state, 0x28) = 0x1E;
    asset = func_8006DC18(0x23);
    magic = 0xCB0000;
    asm volatile("" : "=r"(magic) : "0"(magic));
    state->asset = asset;
    asm volatile("" ::: "memory");
    bank = D_800B0E64;
    asm volatile("" : "=r"(bank) : "0"(bank));
    D_801910B0 = -0x12C;
    D_801910B2 = 0x80;
    D_801910AC = 0;
    D_801910AD = 0;
    D_801910A8 = 0x80;
    D_801910A9 = 0x80;
    D_801910AA = 0x80;
    D_801910AE = 0;
    D_801910B4 = func_8006E498(bank, magic | 0x8704);
}
