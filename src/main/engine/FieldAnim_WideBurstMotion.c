#include "common.h"
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C3B04();

extern u8 D_800E2770[];
extern u16 D_800E27B0;
extern u16 D_800E27B2;
extern u16 D_800E27B4;
extern int D_800E27C0;
extern int D_800E27C4;
extern int D_800E27C8;
extern u16 D_800E27D8;

int func_800CDD0C(void *arg0, void *arg1, u8 *anim) {
    u16 lhs_v0;
    register u16 *output asm("$4");
    u16 rhs_v1;
    unsigned int i;
    u8 *entry;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    lhs_v0 = *(u16 *)(anim + 0x4);
    rhs_v1 = *(u16 *)(anim + 0xA);
    asm("" : : "r"(lhs_v0), "r"(rhs_v1) : "$4");
    output = &D_800E27B0;
    asm("" : "=r"(output) : "0"(output));
    lhs_v0 += rhs_v1;
    *output = lhs_v0;
    D_800E27B2 = *(u16 *)(anim + 0x6) + *(u16 *)(anim + 0xC);
    D_800E27B4 = *(u16 *)(anim + 0x8) + *(u16 *)(anim + 0xE);
    D_800E27C0 = ((s8)anim[3] * 8) + 0x20C;
    D_800E27C4 = ((s8)anim[3] * 8) + 0x20C;
    D_800E27C8 = ((s8)anim[3] * 8) + 0x20C;
    D_800E27D8 = (s8)anim[1];

    func_800C3B04();
    func_800C3098(0x10);

    i = 0;
    do {
        entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);
        *(u16 *)(D_800E2770 + 0x0) = *(u16 *)(entry + 0x10) + *(u16 *)(anim + 0xA);
        *(u16 *)(D_800E2770 + 0x2) = *(u16 *)(entry + 0x20) + *(u16 *)(anim + 0xC);
        *(u16 *)(D_800E2770 + 0x4) = *(u16 *)(entry + 0x30) + *(u16 *)(anim + 0xE);
        i++;
        *(u16 *)(D_800E2770 + 0x28) = (s8)anim[1];
        func_800C3B04(D_800E2770);
    } while ((i & 0xFFFF) < 8);
}


#include "common.h"
void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C3B04();

extern u16 D_800F33E8;
extern u16 D_800F33EA;
extern u16 D_800F33EC;
extern u16 D_800F3410;

int func_800CDE90(void *arg0, void *arg1, u8 *anim) {
    u16 lhs_v0;
    register u16 *output asm("$4");
    u16 rhs_v1;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    lhs_v0 = *(u16 *)(anim + 0x4);
    rhs_v1 = *(u16 *)(anim + 0xA);
    asm("" : : "r"(lhs_v0), "r"(rhs_v1) : "$4");
    output = &D_800F33E8;
    asm("" : "=r"(output) : "0"(output));
    lhs_v0 += rhs_v1;
    *output = lhs_v0;
    D_800F33EA = *(u16 *)(anim + 0x6) + *(u16 *)(anim + 0xC);
    D_800F33EC = *(u16 *)(anim + 0x8) + *(u16 *)(anim + 0xE);
    D_800F3410 = (s8)anim[1];
    func_800C3B04();
}


#include "common.h"

int func_800CDF40(void *arg0, u8 *state) {
    int ret = 2;
    state[1] = ret;
    return ret;
}


#include "common.h"
int func_800CDF4C(void *arg0, u8 *state, u8 *anim) {
    unsigned int i = 0;

    anim[1] -= 0x10;
    anim[3] += 0xC;

    do {
        u8 *entry = (u8 *)(((i & 0xFFFF) * 2) + (int)anim);

        i++;
        *(u16 *)(entry + 0x10) = *(u16 *)(entry + 0x10) + *(u16 *)(entry + 0x40);
        *(u16 *)(entry + 0x20) = *(u16 *)(entry + 0x20) + *(u16 *)(entry + 0x50);
        *(u16 *)(entry + 0x30) = *(u16 *)(entry + 0x30) + *(u16 *)(entry + 0x60);
    } while ((i & 0xFFFF) < 8);

    if ((signed char)anim[1] < 0x10) {
        state[1] = 2;
    }
}


#include "common.h"
int rand(void);

int func_800CDFE0(void *arg0, u8 *state, u8 *anim) {
    register int value asm("$2") = rand() % 11;
    u16 z = *(u16 *)(anim + 0x6);
    register u8 counter asm("$5") = anim[1];
    u16 x = *(u16 *)(anim + 0x4);
    z -= 4;
    counter -= 3;
    x -= 5;
    asm("" : "=r"(x) : "0"(x));

    *(u16 *)(anim + 0x6) = z;
    anim[1] = counter;
    x += value;
    *(u16 *)(anim + 0x4) = x;

    if ((signed char)counter < 3) {
        state[1] = 2;
    }
}
