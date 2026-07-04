/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned int g_SaveMetadataWindowLength;
extern unsigned char *g_SaveMetadataCursor;

unsigned int Save_GetMetadataRemainingChars(void) {
    unsigned int remaining = g_SaveMetadataWindowLength;
    unsigned char *cursor = g_SaveMetadataCursor;

    while (remaining != 0 && *cursor != 0xFF) {
        remaining -= *cursor < 0xFA;
        cursor++;
    }

    return remaining;
}
