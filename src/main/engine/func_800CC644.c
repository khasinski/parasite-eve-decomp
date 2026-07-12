typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3324(void);

extern u16 D_800F32E0;
extern u16 D_800F32E2;
extern u16 D_800F32E4;
extern int D_800F32F0;
extern int D_800F32F4;
extern int D_800F32F8;
extern short D_800F3308;

int func_800CC644(void *arg0, void *arg1, u8 *anim) {
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    D_800F32E0 = *(u16 *)(anim + 0x6);
    D_800F32E2 = *(u16 *)(anim + 0x8);
    D_800F32E4 = *(u16 *)(anim + 0xA);
    D_800F32F0 = *(short *)(anim + 0x4);
    D_800F32F4 = *(short *)(anim + 0x4);
    D_800F32F8 = *(short *)(anim + 0x4);
    D_800F3308 = (signed char)anim[3];
    func_800C3324();
}
