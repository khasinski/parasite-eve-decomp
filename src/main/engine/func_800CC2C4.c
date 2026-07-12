typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;

int rand(void);

extern u16 D_800E2290;
extern u16 D_800E2292;
extern u16 D_800E2294;
extern char *D_8009D254;

int func_800CC2C4(void *arg0, void *arg1, u8 *anim) {
    int i;
    int value;
    u8 *entry;
    char *entity;
    char *core;
    char *mode_data;

    anim[3] = 0x7F;
    entity = D_8009D254;
    core = *(char **)entity;
    mode_data = *(char **)(core + 0x68);
    if (*(short *)(mode_data + 0x6) == 3) {
        *(u16 *)(anim + 0x4) = 4;
    } else {
        *(u16 *)(anim + 0x4) = 0x10;
    }

    if (*(s16 *)(anim + 0x4) > 0) {
        i = 0;
        entry = anim;
        do {
            *(u16 *)(entry + 0x8) = D_800E2290;
            *(u16 *)(entry + 0x28) = D_800E2292;
            *(u16 *)(entry + 0x48) = D_800E2294;

            value = rand();
            *(u16 *)(entry + 0x68) = ((value % 11) - 5) << 8;

            value = rand();
            *(u16 *)(entry + 0x88) = -(((value % 16) + 0x14) << 8);

            value = rand();
            i++;
            *(u16 *)(entry + 0xA8) = ((value % 11) - 5) << 8;
            if (i < *(s16 *)(anim + 0x4)) {
                entry += 2;
            }
        } while (i < *(s16 *)(anim + 0x4));
    }
}
