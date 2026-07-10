/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SaveMetadataWindowLength;
extern unsigned char *g_SaveMetadataCursor;

void Save_AppendMetadataChar(unsigned char ch) {
    register unsigned char *cursor asm("$3");
    register int used asm("$5");
    register int length asm("$6");
    register unsigned int value asm("$2");
    register int has_room asm("$2");
    register int terminator asm("$7");

    /* Retail keeps an otherwise unused 8-byte leaf stack frame. */
    asm volatile("addiu $sp,$sp,-8" ::: "memory");

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
    asm volatile("addiu $sp,$sp,8" ::: "memory");
}
