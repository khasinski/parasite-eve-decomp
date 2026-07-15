/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

typedef unsigned char u8;

extern short *D_8009D07C;
extern u8 D_800BEEAC[];
extern u8 D_8009DE64[];

void *Item_LookupBaseData(unsigned int index);

u8 *Inv_LookupItemData(int index) {
    int value = D_8009D07C[index];
    int saved_value = value;
    u8 *entry;

    if ((unsigned int)(value - 0x100) < 0x80) {
        entry = D_800BEEAC + (value << 5);
    } else if ((unsigned int)(value - 1) < 0xFF) {
        entry = Item_LookupBaseData(value - 1);
    } else if ((unsigned int)(saved_value - 0x200) < 9) {
        entry = D_8009DE64 + (saved_value << 5);
    } else {
        entry = 0;
    }

    return entry;
}
