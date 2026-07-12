typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void *arg0);

extern u16 D_800F3430;
extern u16 D_800F3432;
extern u16 D_800F3434;
extern int D_800F3440;
extern int D_800F3444;
extern int D_800F3448;
extern short D_800F3458;

int func_800CC480(void *arg0, u8 *params, u8 *anim) {
    register short *base_a0 asm("$4");
    register int init_a0 asm("$4") = 3;
    register u8 *anim_s1 asm("$17") = anim;
    register int value_v0 asm("$2");
    register int value_v1 asm("$3");

    func_800C2EAC(init_a0);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v1 = *(u16 *)(params + 0x2);
    value_v0 = 0x80;
    value_v1 <<= 1;
    value_v0 -= value_v1;
    asm volatile("addu %0, %1, $zero" : "=r"(value_v1) : "r"(value_v0));
    value_v0 <<= 16;
    if (value_v0 < 0) {
        value_v1 = 0;
    }

    base_a0 = &D_800F3458;
    base_a0[0] = value_v1;
    asm volatile("" : "=r"(base_a0) : "0"(base_a0));
    base_a0[-20] = *(u16 *)(anim_s1 + 0x6);
    D_800F3432 = *(u16 *)(anim_s1 + 0x8);
    D_800F3434 = *(u16 *)(anim_s1 + 0xA);
    D_800F3440 = *(short *)(anim_s1 + 0x4);
    D_800F3444 = *(short *)(anim_s1 + 0x4) * 2;
    D_800F3448 = *(short *)(anim_s1 + 0x4);
    func_800C3B04(base_a0 - 20);
}
