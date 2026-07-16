/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SaveMetadataWindowLength;
extern unsigned char *g_SaveMetadataCursor;

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
