#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern void *func_8006E498(void *arg0, int arg1);
extern int *func_800C2B28(int arg0);
extern volatile unsigned char D_80192C08;
extern volatile unsigned char D_80192C09;
extern volatile unsigned char D_80192C0A;
extern volatile unsigned char D_80192C0C;
extern volatile unsigned char D_80192C0D;
extern volatile unsigned char D_80192C0E;
extern volatile short D_80192C10;
extern volatile short D_80192C12;
extern volatile unsigned char D_80192C18;
extern volatile unsigned char D_80192C19;
extern volatile unsigned char D_80192C1A;
extern volatile unsigned char D_80192C1C;
extern volatile unsigned char D_80192C1D;
extern volatile unsigned char D_80192C1E;
extern volatile short D_80192C20;
extern volatile short D_80192C22;
extern volatile unsigned char D_80192C28;
extern volatile unsigned char D_80192C29;
extern volatile unsigned char D_80192C2A;
extern volatile unsigned char D_80192C2C;
extern volatile unsigned char D_80192C2D;
extern volatile unsigned char D_80192C2E;
extern volatile short D_80192C30;
extern volatile short D_80192C32;
extern volatile void *D_80192C34;
extern volatile unsigned char D_80192C38;
extern volatile unsigned char D_80192C39;
extern volatile unsigned char D_80192C3A;
extern volatile unsigned char D_80192C3C;
extern volatile unsigned char D_80192C3D;
extern volatile unsigned char D_80192C3E;
extern volatile short D_80192C40;
extern volatile short D_80192C42;

void func_8018F13C(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
    RoomLibFxMatrixWords *matrix;
    void *bank;
    int magic;
    int mode;
    register int c80 asm("$3");
    register int c40 asm("$5");
    register int c20 asm("$7");
    register int c2 asm("$5");
    register int v0val asm("$2");
    register int height asm("$6");

    func_800C2B40(state);
    magic = 0x490000;
    asm volatile("" : "=r"(magic) : "0"(magic));
    bank = D_800B0E64;
    D_80192C34 = func_8006E498(bank, magic | 0x8704);
    state->link = ent->link;
    matrix = (RoomLibFxMatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->asset = func_8006DC18(0xE);
    D_80192C0C = 0;
    D_80192C0D = 0;
    D_80192C10 = 1;
    mode = *func_800C2B28(0);
    if (mode == 2) {
        D_80192C10 = -0x64;
        height = 0x80;
    } else {
        mode = *func_800C2B28(0);
        if (mode == 5) {
            D_80192C10 = -0x64;
        }
        height = 0x80;
    }

    c80 = 0x80;
    c40 = 0x40;
    c20 = 0x20;
    asm volatile("" : "=r"(c80), "=r"(c40), "=r"(c20) : "0"(c80), "1"(c40), "2"(c20));
    v0val = 3;
    D_80192C1D = v0val;
    v0val = -0x64;
    D_80192C20 = v0val;
    v0val = 8;
    D_80192C2C = v0val;
    v0val = 2;
    D_80192C09 = c40;
    D_80192C19 = c40;
    c2 = 2;
    D_80192C2D = v0val;
    v0val = 0x90;
    D_80192C28 = v0val;
    D_80192C29 = v0val;
    D_80192C2A = v0val;
    v0val = 0x10;
    D_80192C12 = height;
    D_80192C0E = 0;
    D_80192C08 = c80;
    D_80192C0A = c20;
    D_80192C1C = c80;
    D_80192C22 = height;
    D_80192C1E = 0;
    D_80192C18 = c80;
    D_80192C1A = c20;
    D_80192C30 = c2;
    D_80192C32 = height;
    D_80192C2E = 0;
    D_80192C3C = 0;
    D_80192C3D = 0;
    D_80192C40 = c2;
    D_80192C3E = 0;
    D_80192C42 = height;
    D_80192C38 = c80;
    D_80192C39 = c20;
    D_80192C3A = v0val;
    asm volatile("" ::: "$4");
    RW8(ent, 2) = *func_800C2B28(0);
}
