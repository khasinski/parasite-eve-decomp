#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern void *func_8006E498(void *arg0, int arg1);
extern unsigned char D_80191BD8;
extern unsigned char D_80191BD9;
extern unsigned char D_80191BDA;
extern unsigned char D_80191BDC;
extern unsigned char D_80191BDD;
extern unsigned char D_80191BDE;
extern short D_80191BE0;
extern short D_80191BE2;
extern void *D_80191BE4;

void func_8018F218(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
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
    D_80191BE0 = -0x12C;
    D_80191BE2 = 0x80;
    D_80191BDC = 0;
    D_80191BDD = 0;
    D_80191BD8 = 0x80;
    D_80191BD9 = 0x80;
    D_80191BDA = 0x80;
    D_80191BDE = 0;
    D_80191BE4 = func_8006E498(bank, magic | 0x8704);
}
