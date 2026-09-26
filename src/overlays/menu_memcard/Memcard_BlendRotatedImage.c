#include "menu_memcard_image.h"
void Memcard_BlendRotatedImage(MemcardImageNode *node, u8 *destination, s32 padding, s32 stride) {
    u8 *out = destination;
    s32 progress = node->targetY;
    MemcardImage *image = node->image;
    u8 *source;
    s32 intensity = node->field1c;
    s32 limit = progress + 24;
    s32 row;
    s32 columns;
    s32 column;
    register s32 product asm("$16");
    s32 eightyFour;
    s32 workspace[1];
    __asm__("" : : "m"(workspace[0]));
    source = (u8 *)image + 20;
    if (60 - progress >= 0) {
        register s32 offset asm("$2");
        product = stride * (60 - progress);
        offset = product << 2;
        out += offset;
    }
    for (row = 0; row < 24; row++, limit--) {
        columns = 84;
        if (limit < 85) columns = limit;
        for (column = 0; column < columns; column++) {
            {
                register s32 input asm("$2") = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
            {
                register s32 input asm("$2") = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
            {
                register s32 input asm("$2") = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
        }
        eightyFour = 84;
        {
            s32 skip = eightyFour - columns;
            source += skip * 3;
            out += padding * 4 + skip * 3;
        }
    }
    image = node->image;
    __asm__("" : "=r"(image) : "0"(image) : "memory");
    source = (u8 *)image + 0x17B1;
    {
        s32 offset = (progress << 1) + progress;
        out = destination + offset;
    }
    {
        s32 count = eightyFour - progress;
        row = 0;
        if (count > 0) {
            register s32 remaining asm("$10");
            columns = count;
            __asm__("" : "=r"(columns) : "0"(columns));
            remaining = 84;
            do {
                u8 *in = source;
                u8 *dst = out;
                for (column = 0; (remaining - progress < 25 ? column < remaining - progress : column < 24); column++) {
                    {
                        register s32 input asm("$2") = in[0];
                        u8 *target = dst;
                        register s32 old asm("$8") = *dst;
                        s32 value;
                        product = input * intensity;
                        input = product >> 8;
                        __asm__("" : "=r"(input), "=r"(dst) : "0"(input), "1"(dst));
                        value = input;
                        dst++;
                        if (value < old) value = old;
                        *target = value;
                    }
                    {
                        register s32 input asm("$2") = in[1];
                        u8 *target = dst;
                        register s32 old asm("$8") = *dst;
                        s32 value;
                        product = input * intensity;
                        input = product >> 8;
                        __asm__("" : "=r"(input), "=r"(dst) : "0"(input), "1"(dst));
                        value = input;
                        dst++;
                        if (value < old) value = old;
                        *target = value;
                    }
                    {
                        register s32 input asm("$2") = in[2];
                        u8 *target = dst;
                        register s32 old asm("$8");
                        register s32 value asm("$3");
                        product = input * intensity;
                        __asm__("" : "=r"(dst), "=r"(target) : "0"(dst), "1"(target));
                        old = *dst;
                        value = product >> 8;
                        dst = target + 1;
                        if (value < old) value = old;
                        *target = value;
                    }
                    in -= 252;
                }
                source -= 3;
                out += stride * 4;
                row++; remaining--;
            } while (row < columns);
        }
    }
}
