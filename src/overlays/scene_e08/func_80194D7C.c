typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

void func_80194D7C(s32 arg0, s16 *rec, u16 *p) {
    s16 v;

    p[8] += p[9];
    p[9] += 10;
    v = p[10];
    if (v >= 9) {
        p[10] = v - 8;
    }
    if (rec[1] == 0x3C) {
        ((s8 *)rec)[1] = 2;
    }
}
