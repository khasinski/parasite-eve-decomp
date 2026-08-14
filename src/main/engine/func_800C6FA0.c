#include "common.h"

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
