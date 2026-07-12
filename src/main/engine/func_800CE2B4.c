typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E2808;
extern u16 D_800E280A;
extern u16 D_800E280C;

int func_800CE2B4(void *arg0, void *arg1, u8 *anim) {
    int value;

    value = rand();
    *(u16 *)(anim + 0x6) = (D_800E2808 - 0x28) + (value % 80);

    value = rand();
    *(u16 *)(anim + 0x8) = (D_800E280A - 0x28) + (value % 80);

    value = rand();
    anim[1] = 0x7F;
    *(u16 *)(anim + 0x4) = 0;
    anim[3] = 0;
    *(u16 *)(anim + 0xA) = (D_800E280C - 0x28) + (value % 80);
}
