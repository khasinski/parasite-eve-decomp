typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);

extern u8 D_800E27E0;
extern u16 D_800E27EA;
extern int D_800F3478;
extern int D_800F348C;
extern int D_800F3490;
extern int D_800F3494;

int func_800CD50C(void *arg0, void *arg1, u8 *anim) {
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    D_800F348C = *(s16 *)(anim + 0x8);
    D_800F3490 = *(s16 *)(anim + 0xA);
    D_800F3494 = *(s16 *)(anim + 0xC);
    D_800E27EA = *(u16 *)(anim + 0x4);
    func_800C42A4(&D_800E27E0, &D_800F3478, 1);
}
