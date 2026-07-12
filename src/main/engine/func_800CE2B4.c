typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E2808;
extern u16 D_800E280A;
extern u16 D_800E280C;

int func_800CE2B4(void *arg0, void *arg1, u8 *anim) {
    register int base asm("$5");
    register int r asm("$2");

    r = rand();
    base = D_800E2808 - 0x28;
    *(u16 *)(anim + 0x6) = base + (r % 80);

    r = rand();
    base = D_800E280A - 0x28;
    *(u16 *)(anim + 0x8) = base + (r % 80);

    r = rand();
    base = D_800E280C;

    anim[1] = 0x7F;
    *(u16 *)(anim + 0x4) = 0;
    anim[3] = 0;
    base -= 0x28;
    *(u16 *)(anim + 0xA) = base + (r % 80);
}
