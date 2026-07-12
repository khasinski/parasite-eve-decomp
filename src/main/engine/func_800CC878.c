typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3324(void);

extern u16 D_800F3338;
extern u16 D_800F333A;
extern u16 D_800F333C;
extern int D_800F3348;
extern int D_800F334C;
extern int D_800F3350;
extern short D_800F3360;

int func_800CC878(void *arg0, void *arg1, u8 *anim) {
    register u16 value_v0 asm("$2");

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm volatile(
        "lui $4,%%hi(D_800F3338)\n\t"
        "addiu $4,$4,%%lo(D_800F3338)\n\t"
        "sh $2,0($4)"
        :
        : "r"(value_v0)
        : "$4", "memory");
    D_800F333A = *(u16 *)(anim + 0x8);
    D_800F333C = *(u16 *)(anim + 0xA);
    D_800F3348 = *(short *)(anim + 0x4);
    D_800F334C = *(short *)(anim + 0x4);
    D_800F3350 = *(short *)(anim + 0x4);
    D_800F3360 = (signed char)anim[3];
    func_800C3324();
}
