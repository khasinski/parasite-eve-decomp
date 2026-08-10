/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/save.h"

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
