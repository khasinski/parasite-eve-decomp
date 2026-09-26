#include "menu_memcard_image.h"
void Memcard_BlendImage(MemcardImageNode *node, u8 *destination, s32 padding) {
    register s32 width asm("$3") = node->width;
    s32 intensity = node->field1c;
    s32 words;
    s32 row;
    u8 *source;
    s32 height;
    s32 column;
    width = (width << 1) + width;
    if (width < 0) width += 3;
    words = width >> 2;

    {
        MemcardImage *image = node->image;
        height = node->height;
        source = (u8 *)image + 20;
    }
    for (row = 0; row < height; row++) {
        for (column = 0; column < words; column++) {
            if (*(u32 *)source) {
                {
                    register u8 *target asm("$9") = destination;
                    s32 old;
                    register s32 input asm("$2") = *source;
                    register s32 product asm("$14");
                    s32 value;

                    product = input * intensity;
                    source++;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    s32 old;
                    register s32 input asm("$2") = *source;
                    register s32 product asm("$14");
                    s32 value;

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    s32 old;
                    register s32 input asm("$2") = *source;
                    register s32 product asm("$14");
                    s32 value;

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    register s32 old asm("$7");
                    register s32 input asm("$2") = *source;
                    register s32 product asm("$14");
                    register s32 value asm("$3");

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    value = product >> 8;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
            } else { source += 4; destination += 4; }
        }
        destination += padding * 4;
    }
}
