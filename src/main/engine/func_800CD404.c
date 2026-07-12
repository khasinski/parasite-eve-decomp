/* CC1_FLAGS: -fno-schedule-insns */

typedef unsigned char u8;
typedef signed short s16;

void func_800C2EAC(int arg0);
void func_800C2FF0(int arg0, int arg1);
void func_800C3098(int arg0);
void func_800C3238(int arg0);
void func_800C42A4(void *arg0, void *arg1, int arg2);

extern u8 D_800E2250;
extern u8 D_800E2254;
extern int D_800F33D4;
extern int D_800F33D8;
extern int D_800F33DC;
extern int D_800F33C0;
extern int D_800F32B0;
extern int D_800F32C4;
extern int D_800F32C8;
extern int D_800F32CC;
extern u8 D_800F3460;

int func_800CD404(void *arg0, void *arg1, u8 *anim) {
    register u8 *anim_s2 asm("$18") = anim;
    register int *field_s1 asm("$17");
    register u8 *color_a3 asm("$7");
    register int *position_v0 asm("$2");
    register u8 *call_a0 asm("$4");
    register int *call_a1 asm("$5");
    register int call_a2 asm("$6");
    register int value_v0 asm("$2");
    register int value_v1 asm("$3");
    register int entry_v0 asm("$2");
    int i;
    int shade;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    color_a3 = &D_800E2254;
    asm volatile("" : "=r"(color_a3) : "0"(color_a3));
    call_a0 = color_a3 - 4;
    asm volatile("" : "=r"(call_a0) : "0"(call_a0));
    position_v0 = &D_800F33D4;
    value_v1 = *(s16 *)(anim_s2 + 0x8);
    call_a1 = position_v0 - 5;
    position_v0[0] = value_v1;
    value_v0 = *(s16 *)(anim_s2 + 0xA);
    call_a2 = 1;
    D_800F33D8 = value_v0;
    value_v0 = *(s16 *)(anim_s2 + 0xC);
    i = 0;
    D_800F33DC = value_v0;
    shade = anim_s2[3];
    asm volatile("" ::: "$17");
    field_s1 = &D_800F32C4;
    *color_a3 = (shade * 2) - 0x80;
    func_800C42A4(call_a0, call_a1, call_a2);

    func_800C3238(3);

    do {
        call_a0 = &D_800F3460;
        asm volatile("" : "=r"(call_a0) : "0"(call_a0));
        entry_v0 = (int)(anim_s2 + ((i & 0xFFFF) * 8));
        value_v1 = *(s16 *)(entry_v0 + 0x10);
        call_a1 = field_s1 - 5;
        field_s1[0] = value_v1;
        value_v1 = *(s16 *)(entry_v0 + 0x12);
        call_a2 = 1;
        field_s1[1] = value_v1;
        value_v0 = *(s16 *)(entry_v0 + 0x14);
        asm volatile("" : "=r"(value_v0) : "0"(value_v0));
        field_s1[2] = value_v0;
        i++;
        func_800C42A4(call_a0, call_a1, call_a2);
    } while ((i & 0xFFFFU) < 2);
}
