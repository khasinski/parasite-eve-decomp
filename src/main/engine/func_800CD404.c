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
    int i;
    u8 *entry;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800F33D4 = *(s16 *)(anim + 0x8);
    D_800F33D8 = *(s16 *)(anim + 0xA);
    D_800F33DC = *(s16 *)(anim + 0xC);
    D_800E2254 = (anim[3] * 2) - 0x80;
    func_800C42A4(&D_800E2250, &D_800F33C0, 1);

    func_800C3238(3);

    i = 0;
    do {
        entry = anim + ((i & 0xFFFF) * 8);
        D_800F32C4 = *(s16 *)(entry + 0x10);
        D_800F32C8 = *(s16 *)(entry + 0x12);
        D_800F32CC = *(s16 *)(entry + 0x14);
        i++;
        func_800C42A4(&D_800F3460, &D_800F32B0, 1);
    } while ((i & 0xFFFF) < 2);
}
