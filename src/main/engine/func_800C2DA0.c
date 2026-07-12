typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *D_800F34F4;
extern char *D_800E2248;

int func_800C2DA0(u16 slot) {
    D_800F34F4[slot * 6 + 1] = 0;
    D_800E2248[6]--;

    return -((((*(int *)(D_800E2248 + 4) & 0xFFFF0000) ^ 0x01000000) < 1));
}
