/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"

static inline ItemDataRecord *LookupTrackedItem(int index) {
    int value, saved;
    ItemDataRecord *result;

    if (index >= 0 && index < D_8009D050) {
        value = D_8009D048[index];
        saved = value;
        if ((unsigned)(value - 0x100) < 0x80) {
            result = &D_800C0E20.equipment[value - 0x100];
        } else {
            if ((unsigned)(value - 1) < 0xFF) {
                return Item_LookupBaseData(value - 1);
            }
            if ((unsigned)(saved - 0x200) < 9) {
                int shifted = saved << 5;
                result = (ItemDataRecord *)(D_8009DE64 + shifted);
            } else {
                result = 0;
            }
        }
        /* Keep the item-ID result merge distinct from an invalid list index. */
        asm("");
        return result;
    }
    return 0;
}

/* Historical name: tests whether a slot may be acted on, excluding equipped
 * weapon/armor slots in Aya's list. An unresolved item remains allowed. */
int Inv_IsSlotEquipped(int index) {
    ItemDataRecord *data;
    int allowed = 1;
    int equipped, kind, count, i;
    data = LookupTrackedItem(index);
    if (data != 0) {
        allowed = 0;
        if (!(data->flags & 0x20)) {
            equipped = 0;
            if (D_8009D048 == D_800C0E48 &&
                (D_800C0E20.tracked[0] == index || D_800C0E20.tracked[2] == index))
                equipped = 1;
            if (!equipped) allowed = 1;
        }
        if (data->kind == 8) {
            kind = 8;
            count = 0;
            for (i = 0; i < D_8009D050; i++) {
                if (Inv_GetActiveListItemType(i) == kind) count++;
            }
            allowed &= count >= 2;
        }
    }
    return allowed;
}
