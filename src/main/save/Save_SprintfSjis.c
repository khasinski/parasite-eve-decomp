#include "pe1/save.h"

void Save_SprintfSjis(unsigned char *dst, unsigned char *fmt)
{
    int *args;
    int ch;
    unsigned char *out;
    int value;
    unsigned int code;
    unsigned char *text;
    int byte;

    out = dst;
    ch = *fmt;
    args = &D_800A1708;
    while (ch != 0) {
        /* lbu already zero-extends; this keeps the retail andi on v1. */
        asm volatile("" : "=r"(ch) : "0"(ch));
        ch &= 0xFF;
        fmt++;
        if (ch == '%') {
            ch = *fmt++;
            switch (ch) {
            case 'd':
                value = *args++;
                code = (value / 10) % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                code = value % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                break;
            case 'D':
                value = *args++;
                code = value % 10 + 0x824Fu;
                *out++ = code >> 8;
                *out++ = (unsigned char)code;
                break;
            case 's':
                text = (unsigned char *)*args++;
                if (text == 0) break;
                byte = *text;
                if (byte == 0) break;
                do {
                    text++;
                    *out++ = byte;
                    byte = *text;
                } while (byte != 0);
                break;
            default:
                break;
            }
        } else {
            *out++ = *(fmt - 1);
        }
        ch = *fmt;
    }
    *out = 0;
}
