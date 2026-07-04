/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_SaveMetadataWindowIndex;
extern unsigned char *g_SaveMetadataCursor;
extern unsigned int g_SaveMetadataWindowLength;
extern void *g_SaveMetadataSourceRecord;
extern unsigned char g_SaveRuntimeState[];
extern unsigned char g_CursorRenderDataBlock[];

void Save_SelectMetadataWindow(void *record) {
    unsigned char *cursor;

    g_SaveMetadataSourceRecord = record;
    if (record != 0) {
        cursor = g_CursorRenderDataBlock;
        if (((unsigned char *)record)[6] == 9) {
            cursor += 0x10;
        }
    } else {
        cursor = g_SaveRuntimeState;
        if (g_SaveMetadataWindowIndex != 0) {
            cursor += 0x10;
        }
    }

    g_SaveMetadataCursor = cursor;
    g_SaveMetadataWindowLength = 8;
}
