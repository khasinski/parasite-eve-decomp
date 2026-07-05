/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef unsigned char u8;

int g_InvItemPtr;
int g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
int g_InvSelectionBits;
int g_InvSelectionBitWords;
extern int g_AyaItemSelectionBits[];
extern short g_AyaInventoryItems[];
extern int g_InvStorageSelectionBits[];

int Inv_GetAyaSlotLimit(void);

void Util_CopyFFTerminatedBytes(u8 *dst, u8 *src)
{
    u8 first;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

void Util_AppendFFTerminatedBytes(u8 *dst, u8 *src)
{
    int scan;
    u8 first;

    scan = *dst++;
    if (scan != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *dst++;
        } while (next != term);
    }

    dst--;

    first = *src++;
    *dst++ = first;
    if (first != 0xFF) {
        int term;
        int next;

        term = 0xFF;
        do {
            next = *src++;
            *dst++ = next;
        } while (next != term);
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/util/tu_0433CC", Inv_BuildItemGridFromCategory);

void Inv_SelectActiveList(int useOverride) {
    if (useOverride != 0 && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = (int)g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = (int)g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = (int)g_AyaItemSelectionBits;
        g_InvSelectionBitWords = 2;
    }
}
