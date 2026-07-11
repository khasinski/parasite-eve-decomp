/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct InvTempItemSlot {
    u8 bytes[0x20];
} InvTempItemSlot;

extern InvTempItemSlot D_800C0EAC[];
extern InvTempItemSlot D_800C1EAC[];
extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

extern s16 *g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");

int Inv_GetAyaSlotLimit(void);
void *Item_LookupBaseData(unsigned int index);

void *Inv_FindSlotByIndex(int item_id) {
    InvTempItemSlot *temp;
    InvTempItemSlot *temp_slot;
    s16 *slot;
    s16 *end;
    u8 *src;
    u8 *dst;
    int temp_index;
    int empty_slot;
    int i;

    temp_slot = 0;

    temp = D_800C0EAC;
    while (temp < D_800C1EAC) {
        if (temp->bytes[0] == 0) {
            break;
        }
        temp++;
    }

    if (temp < D_800C1EAC) {
        temp_index = temp - D_800C0EAC;
    } else {
        temp_index = -1;
    }

    slot = g_InvItemPtr;
    end = slot + g_InvSlotLimit;
    while (slot < end) {
        if (*slot == 0) {
            break;
        }
        slot++;
    }

    if (slot < g_InvItemPtr + g_InvSlotLimit) {
        empty_slot = slot - g_InvItemPtr;
    } else {
        empty_slot = -1;
    }

    if (temp_index >= 0 && empty_slot >= 0) {
        temp_slot = &D_800C0EAC[temp_index];
        src = Item_LookupBaseData(item_id - 1);
        dst = temp_slot->bytes;
        for (i = 0; i < 0x20; i++) {
            dst[i] = src[i];
        }

        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
        g_InvItemPtr[empty_slot] = temp_index + 0x100;
    }

    return temp_slot;
}
