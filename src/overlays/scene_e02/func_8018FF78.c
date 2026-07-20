/* CC1_FLAGS: -fno-strength-reduce */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void func_8018FF78(s32 arg0, s16 *rec, char *p) {
    s16 t;
    u32 i;
    char *e;
    u8 phase;

    *(u16 *)(p + 0x106) += *(u16 *)(p + 0x108);
    t = *(s16 *)(p + 0x102);
    if (t >= 0x11) {
        *(s16 *)(p + 0x102) = t - 0x10;
    }
    i = 0;
    e = p;
    do {
        s32 vx = *(s16 *)(e + 0x80) * *(s16 *)(p + 0x106);
        *(u16 *)(e + 2) += 8;
        *(u16 *)(e + 0) += vx >> 16;
        {
            s32 vz = *(s16 *)(e + 0x84) * *(s16 *)(p + 0x106);
            i++;
            *(u16 *)(e + 4) += vz >> 16;
        }
        e += 8;
    } while (i < 0x10);
    p[0x105] += 6;
    phase = (u8)p[0x105] >> 4;
    p[0x104] = phase + 1;
    if (phase == 7) {
        p[0x104] = 0;
        ((s8 *)rec)[1] = 2;
    }
}
