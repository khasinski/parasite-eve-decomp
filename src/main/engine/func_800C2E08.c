typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *D_800F34F4;
extern char *D_800E2248;

int func_800C2E08(void) {
    int i;
    int result = 0;

    for (i = 0; i < 0x40; i++) {
        u8 *entry = D_800F34F4 + i * 6;

        if ((signed char)entry[1] != 0) {
            entry[1] = 0;
            D_800E2248[6]--;
            result |= -((((*(int *)(D_800E2248 + 4) & 0xFFFF0000) ^ 0x01000000) < 1));
        }
    }

    return result;
}
