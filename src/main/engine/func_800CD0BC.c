typedef unsigned char u8;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E27F8;
extern u16 D_800E27FA;
extern u16 D_800E27FC;

int func_800CD0BC(void *arg0, void *arg1, u8 *anim) {
    u16 *base = &D_800E27F8;
    register u16 *base_s3 asm("$19") = base;
    int i = 0;
    u8 *entry;

    *(u16 *)(anim + 0x8) = base_s3[0];
    *(u16 *)(anim + 0xA) = base_s3[1];
    *(u16 *)(anim + 0x4) = 0x7F;
    *(u16 *)(anim + 0x6) = 0x3B4;
    anim[3] = 0;
    *(u16 *)(anim + 0xC) = base_s3[2];

    do {
        entry = anim + ((i & 0xFFFF) * 8);
        *(u16 *)(entry + 0x10) = base_s3[0];
        *(u16 *)(entry + 0x12) = base_s3[1];
        *(u16 *)(entry + 0x14) = base_s3[2];
        *(u16 *)(entry + 0x20) = (rand() % 50) - 0x19;
        i++;
        *(u16 *)(entry + 0x24) = 0;
        *(u16 *)(entry + 0x22) = (rand() % 50) - 0x19;
    } while ((i & 0xFFFF) < 2);
}
