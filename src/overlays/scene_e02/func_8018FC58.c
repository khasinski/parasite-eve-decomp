/* CC1_FLAGS: -fno-strength-reduce */
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

char *func_800C2B50(void);
s32 func_80077CF4(s32 arg0);
s32 func_80077DC4(s32 arg0);

void func_8018FC58(s32 arg0, s32 arg1, char *p) {
    char *view;
    u32 i;
    s16 *e;
    s32 ang;

    view = func_800C2B50();
    i = 0;
    e = (s16 *)p;
    do {
        ang = i << 9;
        e[0] = *(s32 *)(view + 0x18);
        e[1] = *(s32 *)(view + 0x1C) - 0x64;
        e[2] = *(s32 *)(view + 0x20);
        e[0x40] = func_80077CF4(ang);
        e[0x41] = 0;
        i++;
        e[0x42] = func_80077DC4(ang);
        e += 4;
    } while (i < 0x10);
    *(s16 *)(p + 0x102) = 0x80;
    *(s16 *)(p + 0x100) = 0x400;
    *(s16 *)(p + 0x108) = -0x3C;
    p[0x105] = 0;
    *(s16 *)(p + 0x106) = 0x320;
}
