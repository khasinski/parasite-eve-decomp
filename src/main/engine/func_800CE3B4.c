#include "common.h"
void func_800C2EAC(u8 arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void);

extern u16 D_800E22A8;
extern u16 D_800E22AA;
extern u16 D_800E22AC;
extern short D_800E22D0;
extern signed char D_800E22CC;

int func_800CE3B4(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    register u16 *output asm("$4");
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm("" : : "r"(value_v0) : "$4");
    output = &D_800E22A8;
    asm("" : "=r"(output) : "0"(output));
    *output = value_v0;
    D_800E22AA = *(u16 *)(anim + 0x8);
    D_800E22AC = *(u16 *)(anim + 0xA);
    D_800E22D0 = (signed char)anim[1];
    D_800E22CC = (anim[3] * 2) - 0x60;

    func_800C3B04();
    func_800C3098(0x10);
}
