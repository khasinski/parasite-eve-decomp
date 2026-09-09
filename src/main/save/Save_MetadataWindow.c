/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

void Save_PostInitStub(void) {
}

int Save_GetMetadataWindowIndex(void) {
    return g_SaveMetadataWindowIndex;
}



void Save_SelectMetadataWindow(ItemDataRecord *record) {
    unsigned char *cursor;

    g_SaveMetadataSourceRecord = record;
    if (record != 0) {
        cursor = (u8 *)g_CursorRenderMetadataWindows;
        if (record->kind == ITEM_KIND_ARMOR) {
            cursor += 0x10;
        }
    } else {
        cursor = (u8 *)g_SaveRuntimeMetadataWindows;
        if (g_SaveMetadataWindowIndex != 0) {
            cursor += 0x10;
        }
    }

    g_SaveMetadataCursor = cursor;
    g_SaveMetadataWindowLength = 8;
}



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



int Save_DeleteMetadataChar(void) {
    register unsigned char *cursor asm("$4") = g_SaveMetadataCursor;
    int used = 0;
    int length = g_SaveMetadataWindowLength;

    while (used < length && *cursor != 0xFF) {
        used += *cursor < 0xFA;
        cursor++;
    }

    used = 0 < used;
    if (used != 0) {
        cursor--;
        cursor[0] = 0xFF;
        cursor--;
        if (cursor >= g_SaveMetadataCursor && *cursor >= 0xFA) {
            cursor[0] = 0xFF;
        }
    }

    return used;
}



unsigned char *Str_LookupTable4(unsigned int index);
unsigned char *Str_LookupTable8(unsigned int index);

void Save_LoadMetadataWindowText(void) {
    unsigned char *cursor;
    unsigned char *text;
    unsigned char *lookup;
    unsigned char *dst;
    unsigned char *out;
    unsigned char value;
    unsigned char loop_value;
    int loop_terminator;

    cursor = g_SaveMetadataCursor;
    if (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength) {
        do {
            *cursor = 0xFF;
            cursor++;
        } while (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength);
    }

    dst = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord != 0) {
        lookup = Str_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
        out = dst;
    } else {
        lookup = Str_LookupTable4(0x1E);
        out = dst;
    }

    text = lookup;
    value = *text++;
    *out++ = value;
    if (value != 0xFF) {
        loop_terminator = 0xFF;
        do {
            loop_value = *text++;
            *out++ = loop_value;
        } while (loop_value != loop_terminator);
    }
}



unsigned char *Save_GetMetadataCursor(void) {
    return g_SaveMetadataCursor;
}



char *Save_GetActiveMetadataBuffer(void) {
    char *buffer;

    buffer = (char *)g_SaveRuntimeMetadataWindows;

    if (g_SaveMetadataWindowIndex != 0) {
        buffer += 0x10;
    }

    return buffer;
}



unsigned int Save_GetMetadataRemainingChars(void) {
    unsigned int remaining = g_SaveMetadataWindowLength;
    unsigned char *cursor = g_SaveMetadataCursor;

    while (remaining != 0 && *cursor != 0xFF) {
        remaining -= *cursor < 0xFA;
        cursor++;
    }

    return remaining;
}
