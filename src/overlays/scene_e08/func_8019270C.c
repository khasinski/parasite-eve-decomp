typedef signed char s8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

void func_8019270C(s32 arg0, s16 *rec, s16 *p) {
    u16 *q;
    s16 pad;

    q = (u16 *)p;
    if (p[8] < 0x960) {
        p[8] += 0x28;
    }
    if (rec[1] >= 0x3D) {
        q[9] += 0x190;
        if ((s16)q[9] >= 0x1449) {
            q[9] = 0x1448;
        }
    }
    if (rec[1] >= 0x6A) {
        ((s8 *)rec)[1] = 2;
    }
}
