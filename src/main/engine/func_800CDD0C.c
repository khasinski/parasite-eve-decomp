typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;

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
    register u16 lhs_v0 asm("$2");
    register u16 rhs_v1 asm("$3");
    unsigned int i;
    u8 *entry;

    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    lhs_v0 = *(u16 *)(anim + 0x4);
    rhs_v1 = *(u16 *)(anim + 0xA);
    asm volatile(
        "lui $4,%%hi(D_800E27B0)\n\t"
        "addiu $4,$4,%%lo(D_800E27B0)\n\t"
        "addu $2,$2,$3\n\t"
        "sh $2,0($4)"
        : "=r"(lhs_v0)
        : "0"(lhs_v0), "r"(rhs_v1)
        : "$4", "memory");
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
