#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern void *func_8006E498(void *arg0, int arg1);
extern unsigned char D_80190E90;
extern unsigned char D_80190E91;
extern unsigned char D_80190E92;
extern unsigned char D_80190E94;
extern unsigned char D_80190E95;
extern unsigned char D_80190E96;
extern short D_80190E98;
extern short D_80190E9A;
extern void *D_80190E9C;

void func_8018F1D4(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
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
    D_80190E98 = -0x12C;
    D_80190E9A = 0x80;
    D_80190E94 = 0;
    D_80190E95 = 0;
    D_80190E90 = 0x80;
    D_80190E91 = 0x80;
    D_80190E92 = 0x80;
    D_80190E96 = 0;
    D_80190E9C = func_8006E498(bank, magic | 0x8704);
}
