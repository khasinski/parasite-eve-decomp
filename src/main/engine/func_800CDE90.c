typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;

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
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    D_800F33E8 = *(u16 *)(anim + 0x4) + *(u16 *)(anim + 0xA);
    D_800F33EA = *(u16 *)(anim + 0x6) + *(u16 *)(anim + 0xC);
    D_800F33EC = *(u16 *)(anim + 0x8) + *(u16 *)(anim + 0xE);
    D_800F3410 = (s8)anim[1];
    func_800C3B04();
}
