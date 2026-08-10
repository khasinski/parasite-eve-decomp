/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

void Save_AppendMetadataChar(unsigned char ch) {
    unsigned char *cursor;
    int used;
    int length;
    unsigned int value;
    int has_room;
    int terminator;
    int frame;

    /* Retail keeps an otherwise unused 8-byte leaf stack frame. */
    asm volatile("" : : "r"(&frame));

    length = g_SaveMetadataWindowLength;
    cursor = g_SaveMetadataCursor;
    if (length > 0) {
        used = 0;
        terminator = 0xFF;

loop:
        value = *cursor;
        if (value != terminator) {
            used += value < 0xFA;
            cursor++;
            if (used < length) {
                goto loop;
            }
        }

        has_room = used < g_SaveMetadataWindowLength;
        if (has_room != 0) {
            cursor[0] = ch;
            cursor[1] = 0xFF;
            goto done;
        }
    }

    if (cursor[-2] >= 0xFA) {
        cursor--;
        cursor[0] = 0xFF;
    }

    cursor[-1] = ch;

done:
    }
