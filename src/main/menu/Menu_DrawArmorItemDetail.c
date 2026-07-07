/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;

extern s16 *D_8009D07C;
extern unsigned int *D_8009D058;
extern u8 g_EquipItemDataTable[];
extern u8 g_KeyItemDataTable[];
extern u8 D_800C20A4[];

void Draw_SetTextDimmed(int value);
void *Item_LookupBaseData(unsigned int index);
void *Str_LookupTable8(unsigned int index);
void Sfx_DrawSlotRow(void *entry, void *display);

void Menu_DrawArmorItemDetail(int slot) {
    void *display;
    u8 *entry;
    u8 *candidate;
    int value;
    unsigned int bit;

    value = D_8009D07C[slot];
    if (value != 0) {
        bit = D_8009D058[slot >> 5] & (1 << (slot & 0x1F));
        Draw_SetTextDimmed(bit == 0);

        if ((unsigned int)(value - 0x100) < 0x80U) {
            candidate = g_EquipItemDataTable + (value << 5);
            goto resolved;
        } else if ((unsigned int)(value - 1) < 0xFFU) {
            candidate = Item_LookupBaseData(value - 1);
            goto resolved;
        } else if ((unsigned int)(value - 0x200) < 9U) {
            candidate = g_KeyItemDataTable + (value << 5);
            goto resolved;
        } else {
            candidate = 0;
        }

resolved:
        entry = candidate;

        if ((entry[5] & 0x10) != 0) {
            display = D_800C20A4;
            if (entry[6] == 9) {
                display = D_800C20A4 + 0x10;
            }
        } else {
            display = Str_LookupTable8(entry[4] - 1);
        }

        Sfx_DrawSlotRow(entry, display);
    }
}
