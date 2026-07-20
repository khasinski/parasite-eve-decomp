typedef signed char s8;
typedef short s16;
typedef int s32;

char *func_800C2B50(void);
s32 func_800C5EB0(void *arg0, s16 *arg1, s32 *arg2);

extern char *D_8009D254;

void func_80190BA0(s32 arg0, s16 *rec, char *p) {
    char *view;
    s16 ang[3];
    s32 res[2];
    s32 a, b;
    s32 t0, t1, t2;

    view = func_800C2B50();
    t0 = *(s16 *)(D_8009D254 + 0x2A);
    ang[0] = t0;
    t1 = *(s16 *)(D_8009D254 + 0x2A);
    ang[1] = t1;
    t2 = *(s16 *)(D_8009D254 + 0x2A);
    ang[2] = t2;
    a = func_800C5EB0(p + 0xAC, ang, res);
    b = func_800C5EB0(p + 0x78, ang, res);
    if (res[0] == 1) {
        *(s16 *)(view + 0x68) = 1;
    }
    if (b == 1) {
        ((s8 *)rec)[1] = 2;
    }
}
