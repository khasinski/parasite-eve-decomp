#include "../room_lib/room_lib.h"

extern void *D_800B0E64;
extern u8 D_80192C08;
extern u8 D_80192C09;
extern u8 D_80192C0A;
extern u8 D_80192C0C;
extern u8 D_80192C0D;
extern u8 D_80192C0E;
extern s16 D_80192C10;
extern s16 D_80192C12;
extern u8 D_80192C18;
extern u8 D_80192C19;
extern u8 D_80192C1A;
extern u8 D_80192C1C;
extern u8 D_80192C1D;
extern u8 D_80192C1E;
extern s16 D_80192C20;
extern s16 D_80192C22;
extern u8 D_80192C28;
extern u8 D_80192C29;
extern u8 D_80192C2A;
extern u8 D_80192C2C;
extern u8 D_80192C2D;
extern u8 D_80192C2E;
extern s16 D_80192C30;
extern s16 D_80192C32;
extern void *D_80192C34;
extern u8 D_80192C38;
extern u8 D_80192C39;
extern u8 D_80192C3A;
extern u8 D_80192C3C;
extern u8 D_80192C3D;
extern u8 D_80192C3E;
extern s16 D_80192C40;
extern s16 D_80192C42;

extern void *func_8006E498(void *base, int id);
extern int *func_800C2B28(int index);

void func_8018F13C(RoomEnt *ent, void *unused, RoomLibFxMatrixState *state) {
    RoomLink *link;
    RoomLibFxMatrixWords *matrix;
    register int c80a asm("$6");
    register int c80b asm("$3");
    register int c40 asm("$5");
    int c20;
    register int c2 asm("$2");

    func_800C2B40(state);
    D_80192C34 = func_8006E498(D_800B0E64, 0x498704);
    link = ent->link;
    state->link = link;
    matrix = (RoomLibFxMatrixWords *)link->p238;
    state->matrix = *matrix;
    state->asset = func_8006DC18(0xE);

    D_80192C0C = 0;
    D_80192C0D = 0;
    D_80192C10 = 1;
    if (*func_800C2B28(0) == 2 || *func_800C2B28(0) == 5) {
        D_80192C10 = -100;
    }
    c80a = 0x80;
    c80b = 0x80;
    PE1_COMPILER_USE(c80b);
    c40 = 0x40;
    c20 = 0x20;
    PE1_COMPILER_USE(c20);
    D_80192C1D = 3;
    D_80192C20 = -100;
    D_80192C2C = 8;
    PE1_COMPILER_MEMORY_BARRIER();
    c2 = 2;
    D_80192C09 = c40;
    D_80192C19 = c40;
    c40 = 2;
    D_80192C2D = c2;
    D_80192C28 = 0x90;
    D_80192C29 = 0x90;
    D_80192C2A = 0x90;
    D_80192C12 = c80a;
    D_80192C0E = 0;
    D_80192C08 = c80b;
    D_80192C0A = c20;
    D_80192C1C = c80b;
    D_80192C22 = c80a;
    D_80192C1E = 0;
    D_80192C18 = c80b;
    D_80192C1A = c20;
    D_80192C30 = c40;
    D_80192C32 = c80a;
    D_80192C2E = 0;
    D_80192C3C = 0;
    D_80192C3D = 0;
    D_80192C40 = c40;
    D_80192C3E = 0;
    D_80192C42 = c80a;
    D_80192C38 = c80b;
    D_80192C39 = c20;
    D_80192C3A = 0x10;
    ent->pad1[1] = *func_800C2B28(0);
}
