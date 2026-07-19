typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

s32 func_80077DC4(s32 arg0);

void func_80191350(s32 arg0, s16 *rec, u16 *p) {
    u16 *q;
    s16 zero;
    s32 step;

    q = p;
    step = func_80077DC4(rec[1] << 5) >> 6;
    p[4] += step;
    ((u8 *)p)[0xC]++;
    p[5] -= 4;
    zero = 0;
    if ((s16)p[5] < 0) {
        p[5] = zero;
    }
    if ((s16)q[5] == zero) {
        ((s8 *)rec)[1] = 2;
    }
}
