typedef unsigned char u8;

extern unsigned char g_CursorRenderDataBlock[];

void *Str_LookupTable8(unsigned int arg0);
void Sfx_DrawSlotRow(void *record, unsigned char *cursor);

void Sfx_CursorRenderData(void *record) {
    u8 *cursor;

    if (((u8 *) record)[5] & 0x10) {
        cursor = g_CursorRenderDataBlock;
        if (((u8 *) record)[6] == 9) {
            cursor = g_CursorRenderDataBlock + 0x10;
        }
    } else {
        cursor = Str_LookupTable8(((u8 *) record)[4] - 1);
    }

    Sfx_DrawSlotRow(record, cursor);
}
