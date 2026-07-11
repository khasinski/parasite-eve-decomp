/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");

int Inv_GetAyaSlotLimit(void);

int Inv_FindFirstEmptySlot(int required_slots) {
    int limit;
    int run_length;
    s16 *base;
    s16 *slot;
    s16 *last_start;
    s16 *scan;

    g_InvItemPtr = (s32)g_AyaInventoryItems;
    limit = Inv_GetAyaSlotLimit();
    base = (s16 *)g_InvItemPtr;
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSlotLimit = limit;
    g_InvSelectionBitWords = 2;

    last_start = base + limit - required_slots;
    if (base >= last_start + 1) {
        return -1;
    }

    slot = base;
    while (slot < last_start + 1) {
        if (*slot != 0) {
            slot++;
            continue;
        }

        run_length = 1;
        if (run_length < required_slots) {
            scan = slot + 1;
            while (*scan == 0) {
                run_length++;
                if (run_length >= required_slots) {
                    break;
                }
                scan++;
            }
        }

        if (run_length >= required_slots) {
            return slot - base;
        }

        slot++;
    }

    return -1;
}
