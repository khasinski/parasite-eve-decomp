#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern void *func_8006E498(void *arg0, int arg1);
extern unsigned char D_80190188;
extern unsigned char D_80190189;
extern unsigned char D_8019018A;
extern unsigned char D_8019018C;
extern unsigned char D_8019018D;
extern unsigned char D_8019018E;
extern short D_80190190;
extern short D_80190192;
extern void *D_80190194;

void func_8018F1BC(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
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
    D_80190190 = -0x12C;
    D_80190192 = 0x80;
    D_8019018C = 0;
    D_8019018D = 0;
    D_80190188 = 0x80;
    D_80190189 = 0x80;
    D_8019018A = 0x80;
    D_8019018E = 0;
    D_80190194 = func_8006E498(bank, magic | 0x8704);
}
