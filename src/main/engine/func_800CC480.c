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
    int value;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value = *(u16 *)(params + 0x2);
    value = 0x80 - (value * 2);
    if ((value << 16) < 0) {
        value = 0;
    }

    base_a0 = &D_800F3458;
    base_a0[0] = value;
    base_a0[-20] = *(u16 *)(anim + 0x6);
    D_800F3432 = *(u16 *)(anim + 0x8);
    D_800F3434 = *(u16 *)(anim + 0xA);
    D_800F3440 = *(short *)(anim + 0x4);
    D_800F3444 = *(short *)(anim + 0x4) * 2;
    D_800F3448 = *(short *)(anim + 0x4);
    func_800C3B04(base_a0 - 20);
}
