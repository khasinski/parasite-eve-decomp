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
    int i;
    int shade;
    u8 *entry;
    int *position;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    color_a3 = &D_800E2254;
    position = &D_800F33D4;
    position[0] = *(s16 *)(anim_s2 + 0x8);
    position[1] = *(s16 *)(anim_s2 + 0xA);
    position[2] = *(s16 *)(anim_s2 + 0xC);
    shade = anim_s2[3];
    field_s1 = &D_800F32C4;
    *color_a3 = (shade * 2) - 0x80;
    func_800C42A4(color_a3 - 4, position - 5, 1);

    func_800C3238(3);

    i = 0;
    do {
        entry = anim_s2 + ((i & 0xFFFF) * 8);
        field_s1[0] = *(s16 *)(entry + 0x10);
        field_s1[1] = *(s16 *)(entry + 0x12);
        field_s1[2] = *(s16 *)(entry + 0x14);
        i++;
        func_800C42A4(&D_800F3460, field_s1 - 5, 1);
    } while ((i & 0xFFFF) < 2);
}
