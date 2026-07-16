/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned char *g_SaveMetadataCursor;
extern unsigned int g_SaveMetadataWindowLength;
extern unsigned char *g_SaveMetadataSourceRecord;

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
        lookup = Str_LookupTable8(g_SaveMetadataSourceRecord[4] - 1);
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
