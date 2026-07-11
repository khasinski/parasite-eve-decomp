/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;

extern u32 D_800A1708[];

void Save_SprintfSjis(u8 *dst, u8 *fmt) {
    u8 ch;
    u8 *out;
    u32 *arg;
    int value;
    int tens;
    int digit;
    int sjis;
    u8 *str;

    arg = D_800A1708;
    ch = *fmt;
    out = dst;
    if (ch != 0) {
        do {
            ch &= 0xFF;
            if (ch == '%') {
                ch = *++fmt;
                fmt++;
                if (ch == 'd') {
                    value = *arg++;
                    tens = value / 10;
                    digit = tens % 10;
                    sjis = digit + 0x824F;
                    *out++ = sjis >> 8;
                    *out++ = sjis;
                    digit = value - (tens * 10);
                    sjis = digit + 0x824F;
                    *out++ = sjis >> 8;
                    *out++ = sjis;
                } else if (ch == 'D') {
                    value = *arg++;
                    digit = value % 10;
                    sjis = digit + 0x824F;
                    *out++ = sjis >> 8;
                    *out++ = sjis;
                } else if (ch == 's') {
                    str = (u8 *)*arg++;
                    if (str != 0) {
                        ch = *str;
                        if (ch != 0) {
                            do {
                                str++;
                                *out++ = ch;
                                ch = *str;
                            } while (ch != 0);
                        }
                    }
                }
            } else {
                fmt++;
                *out++ = fmt[-1];
            }
            ch = *fmt;
        } while (ch != 0);
    }

    *out = 0;
}
