/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

extern s16 g_AyaInventoryItems[] __asm__("D_800C0E48");
extern u8 g_AyaInventorySlotCount[0x40] __asm__("D_800C0E0C");
extern u32 g_AyaItemSelectionBits[] __asm__("D_8009D05C");

extern s32 g_InvItemPtr __asm__("D_8009D048");
extern s32 g_InvSlotLimit __asm__("D_8009D050");
extern u32 *g_InvSelectionBits __asm__("D_8009D058");
extern s32 g_InvSelectionBitWords __asm__("D_8009D064");
extern s32 D_8009D06C;

int Inv_GetAyaSlotLimit(void);
int Inv_GetBonusSlotCount(void);

int Inv_CheckFreeSlotCapacity(int requested) {
    int limit;
    register int used_slots asm("$16");
    register int max_slots asm("$17");
    register int requested_slots asm("$18");
    int item;
    s16 *slot;
    s16 *end;

    requested_slots = requested;
    g_InvItemPtr = (s32)g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSelectionBitWords = 2;

    used_slots = 0;
    if (g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount() < 0x33) {
        max_slots = g_AyaInventorySlotCount[0] + Inv_GetBonusSlotCount();
    } else {
        max_slots = 0x32;
    }

    g_InvItemPtr = (s32)g_AyaInventoryItems;
    limit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_AyaItemSelectionBits;
    g_InvSlotLimit = limit;
    g_InvSelectionBitWords = 2;
    slot = (s16 *)g_InvItemPtr;
    end = slot + limit;

    while (slot < end) {
        item = *slot;
        slot++;
        used_slots += (u32)item > 0;
    }

    D_8009D06C = requested_slots;

    return max_slots - used_slots >= requested_slots;
}
