#include "common.h"
extern u32 D_800E2370[];

void func_800C6EF8(char *data) {
    int i = 0;
    u32 *src = (u32 *)(data + *(u16 *)(data + 0x8));
    u32 *dst = D_800E2370;

    while (i < *(u16 *)(data + 0xA)) {
        i++;
        *dst++ = *src++;
    }
}


void func_800C6F4C(char *data) {
    int i = 0;
    u32 *src = D_800E2370;
    u32 *dst = (u32 *)(data + *(u16 *)(data + 0x8));

    while (i < *(u16 *)(data + 0xA)) {
        i++;
        *dst++ = *src++;
    }
}


typedef struct ColorEntry {
    u8 red;
    u8 green;
    u8 blue;
    u8 unused;
} ColorEntry;

typedef struct ColorTable {
    u8 unused[8];
    u16 entries_offset;
    u16 entry_count;
} ColorTable;

void func_800C6FA0(char *data, u16 factor)
{
    int i;
    u32 red;
    u32 blue;
    u32 blue_product;
    u32 green;
    u32 green_product;
    ColorTable *table = (ColorTable *)data;
    volatile u32 *scratch = (volatile u32 *)0x1F800000;
    ColorEntry *entry = (ColorEntry *)(data + table->entries_offset);

    for (i = 0; i < table->entry_count; i++, entry++) {
        red = entry->red;
        red *= factor;
        scratch[12] = red;
        if (red > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        red = scratch[12] >> 7;
        scratch[12] = red;
        entry->red = red;

        green_product = entry->green;
        green_product *= factor;
        scratch[12] = green_product;
        if (green_product > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        green = scratch[12] >> 7;
        scratch[12] = green;
        entry->green = green;

        blue_product = entry->blue;
        blue_product *= factor;
        scratch[12] = blue_product;
        if (blue_product > 0x7FFF) {
            scratch[12] = 0x7FFF;
        }
        blue = scratch[12] >> 7;
        scratch[12] = blue;
        entry->blue = blue;
    }
}


void func_800C7098(char *data, u8 r, u8 g, u8 b) {
    int i = 0;
    register int count asm("$3");
    u8 *dst = (u8 *)(data + *(u16 *)(data + 0x8));
    /* Keeps the empty 8-byte frame emitted by the original. */
    volatile int pad;

    count = *(u16 *)(data + 0xA);
    if (count > 0) {
        do {
            i++;
            dst[0] = r;
            dst[1] = g;
            dst[2] = b;
            dst += 4;
        } while (i < *(u16 *)(data + 0xA));
    }
}

void func_800C70EC(char *data, int rDelta, int gDelta, int bDelta) {
    int frame;
    register int offset asm("$2");
    int count;
    int i;
    u8 *entry;
    register u8 *bluePtr asm("$9");

        i = 0;
        offset = *(u16 *)(data + 0x8);
        count = *(u16 *)(data + 0xA);
    entry = (u8 *)(data + offset);
    if (count > 0) {
        bluePtr = entry + 2;
        do {
            register s16 value asm("$3");
            int calc;
            int blueValue;
            register int blue asm("$8");

            value = entry[0];
            calc = value + rDelta;
            value = calc;
            if ((s16)calc >= 0x100) {
                value = 0xFF;
            }
            if (value < 0) {
                value = 0;
            }
            entry[0] = value;

            value = bluePtr[-1];
            calc = value + gDelta;
            value = calc;
            if ((s16)calc >= 0x100) {
                value = 0xFF;
            }
            if (value < 0) {
                value = 0;
            }

            blue = bluePtr[0];
            blueValue = blue + bDelta;
            blue = blueValue;
            asm volatile("" : "=r"(blueValue) : "0"(blueValue));
            bluePtr[-1] = value;
            if ((s16)blueValue >= 0x100) {
                blue = 0xFF;
            }
            if ((s16)blue < 0) {
                blue = 0;
            }
            i++;
            bluePtr[0] = blue;
            bluePtr += 4;
            entry += 4;
        } while (i < *(u16 *)(data + 0xA));
    }
    asm volatile("" : : "r"(&frame));
}
