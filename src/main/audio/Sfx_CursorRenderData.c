#include "common.h"
#include "pe1/inventory.h"
extern unsigned char g_CursorRenderDataBlock[];

void *Str_LookupTable8(unsigned int arg0);
void Sfx_DrawSlotRow(ItemDataRecord *record, unsigned char *cursor);

void Sfx_CursorRenderData(ItemDataRecord *record) {
    u8 *cursor;

    if (record->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) {
        cursor = g_CursorRenderDataBlock;
        if (record->kind == ITEM_KIND_ARMOR) {
            cursor = g_CursorRenderDataBlock + 0x10;
        }
    } else {
        cursor = Str_LookupTable8(record->itemId - 1);
    }

    Sfx_DrawSlotRow(record, cursor);
}
