typedef unsigned char u8;
typedef unsigned short u16;

void func_800C2EAC(int arg0);
void func_800C3098(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3238(int arg0);
void func_800C3B04(void);

extern u16 D_800F3380;
extern u16 D_800F3382;
extern u16 D_800F3384;
extern int D_800F3390;
extern int D_800F3394;
extern int D_800F3398;
extern short D_800F33A8;

int func_800CC6F8(void *arg0, void *arg1, u8 *anim) {
    register u16 value_v0 asm("$2");

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    asm volatile(
        "lui $4,%%hi(D_800F3380)\n\t"
        "addiu $4,$4,%%lo(D_800F3380)\n\t"
        "sh $2,0($4)"
        :
        : "r"(value_v0)
        : "$4", "memory");
    D_800F3382 = *(u16 *)(anim + 0x8);
    D_800F3384 = *(u16 *)(anim + 0xA);
    D_800F3390 = *(short *)(anim + 0x4);
    D_800F3394 = *(short *)(anim + 0x4);
    D_800F3398 = *(short *)(anim + 0x4);
    D_800F33A8 = (signed char)anim[3];
    func_800C3B04();
}
