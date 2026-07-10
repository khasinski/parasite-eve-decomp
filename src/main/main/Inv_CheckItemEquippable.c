typedef signed char s8;
typedef signed short s16;
typedef unsigned char u8;

extern s16 *D_8009D048;
extern int D_8009D050;
extern u8 D_800BEEAC[];
extern u8 D_8009DE64[];
extern s16 D_800C0E48[];
extern struct { char _[16]; } D_800C0E22_obj __asm__("D_800C0E22");

#define D_800C0E22 (*(s8 *)&D_800C0E22_obj)

int Inv_RestoreSelection(unsigned int index);
void *Item_LookupBaseData(unsigned int index);
int Inv_CheckFreeSlotCapacity(int mask);

int Inv_CheckItemEquippable(unsigned int list_index, int modifier_index) {
    int selected_a;
    int selected_b;
    u8 *item_a;
    u8 *item_b;
    int item_id;
    int count;
    int i;
    int result;
    int mask;
    int modifier;
    int existing;
    int different_slot;

    mask = 0;
    selected_a = Inv_RestoreSelection(0);
    item_a = 0;
    if (selected_a >= 0 && selected_a < D_8009D050) {
        item_id = D_8009D048[selected_a];
        if ((unsigned int)(item_id - 0x100) < 0x80) {
            item_a = D_800BEEAC + (item_id << 5);
        } else if ((unsigned int)(item_id - 1) < 0xFF) {
            item_a = Item_LookupBaseData(item_id - 1);
        } else if ((unsigned int)(item_id - 0x200) < 9) {
            item_a = D_8009DE64 + (item_id << 5);
        }
    }

    selected_b = Inv_RestoreSelection(list_index < 1);
    item_b = 0;
    if (selected_b >= 0 && selected_b < D_8009D050) {
        item_id = D_8009D048[selected_b];
        if ((unsigned int)(item_id - 0x100) < 0x80) {
            item_b = D_800BEEAC + (item_id << 5);
        } else if ((unsigned int)(item_id - 1) < 0xFF) {
            item_b = Item_LookupBaseData(item_id - 1);
        } else if ((unsigned int)(item_id - 0x200) < 9) {
            item_b = D_8009DE64 + (item_id << 5);
        }
    }

    result = 1;
    if (modifier_index >= 0) {
        modifier = item_a[modifier_index + 0x15];
        count = item_b[0x14];
        for (i = 0; i < count; i++) {
            if (item_b[i + 0x15] == modifier) {
                return 4;
            }
        }

        for (i = 0; i < item_b[0x14]; i++) {
            if (item_b[i + 0x15] == 0) {
                break;
            }
        }

        if (i >= item_b[0x14]) {
            modifier &= 0xE0;
            if (modifier == 0) {
                return 3;
            }

            for (i = 0; i < item_b[0x14]; i++) {
                if ((item_b[i + 0x15] & 0xE0) == modifier) {
                    break;
                }
            }

            if (i >= item_b[0x14]) {
                return 3;
            }
        }
    }

    Inv_RestoreSelection(list_index);
    if (D_8009D048 == D_800C0E48 && D_800C0E22 == selected_a) {
        count = item_a[0x14];
        for (i = 0; i < count; i++) {
            modifier = (item_a[i + 0x15] & 0x1F) - 8;
            if ((unsigned int)modifier < 3) {
                break;
            }
        }

        if (i < item_a[0x14]) {
            mask = 1 << modifier;
            different_slot = i != modifier_index;
            result = Inv_CheckFreeSlotCapacity(mask);
            if (result != 0) {
                return result;
            }
            if (different_slot != 0 || modifier_index < 0) {
                result = 5;
            }
        }
        return result;
    }

    Inv_RestoreSelection(list_index < 1);
    if (D_8009D048 != D_800C0E48) {
        return 1;
    }
    if (D_800C0E22 != selected_b) {
        return 1;
    }
    if (modifier_index < 0) {
        return 1;
    }

    modifier = (item_a[modifier_index + 0x15] & 0x1F) - 8;
    if ((unsigned int)modifier >= 3) {
        return 1;
    }

    count = item_b[0x14];
    for (i = 0; i < count; i++) {
        existing = (item_b[i + 0x15] & 0x1F) - 8;
        if ((unsigned int)existing < 3) {
            mask = (1 << existing) - (1 << modifier);
            break;
        }
    }

    result = Inv_CheckFreeSlotCapacity(mask);
    return result;
}
