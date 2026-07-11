typedef signed char s8;
typedef signed short s16;
typedef unsigned short u16;

extern char *func_800C2B50(void);

void func_8018FFFC(char *arg0, char *arg1, char *arg2) {
    char *state;
    volatile s16 pos[3];
    int temp;
    s16 scale;

    state = func_800C2B50();
    temp = *(s16 *)(*(char **)(arg0 + 0x8) + 0x2A);
    pos[0] = temp;
    temp = *(s16 *)(*(char **)(arg0 + 0x8) + 0x32);
    pos[2] = temp;

    if (*(s16 *)(arg1 + 0x2) < 0x20) {
        *(u16 *)(arg2 + 0x138) += *(u16 *)(arg2 + 0x13C);
        *(u16 *)(arg2 + 0x13C) += 7;
        if (*(s16 *)(arg1 + 0x2) < 0x20) {
            scale = *(s16 *)(arg2 + 0x13A);
            if (scale >= 5) {
                *(s16 *)(arg2 + 0x13A) = scale - 4;
            }
        }
    }

    if (*(s16 *)(arg1 + 0x2) == 0x10) {
        *(s16 *)(state + 0x2C) = 1;
    }
    if (*(s16 *)(arg1 + 0x2) == 0x20) {
        arg1[1] = 2;
    }
}
