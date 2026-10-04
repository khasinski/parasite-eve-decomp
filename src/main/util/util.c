#include "common.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_InvItemPtr;
int g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
int g_InvSelectionBits;
int g_InvSelectionBitWords;
extern int g_AyaItemSelectionBits[];
extern short g_AyaInventoryItems[];
extern int g_InvStorageSelectionBits[];
extern int D_8009D03C;
extern int D_8009D04C;
extern s16 D_800C0EAA[];
extern s16 D_800C1F7E[];
extern s16 D_800C2022[];
extern u8 D_800A1E6D[][32];
extern s16 D_800A1E76[][16];

extern ItemDataRecord D_800A1E64[];

int Inv_GetAyaSlotLimit(void);
void Inv_SelectActiveList(int useOverride);

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

void Inv_BuildItemGridFromCategory(void)
{
    int i;
    s16 *clear;
    ItemDataRecord *src;
    int category;
    int base;
    int lookup;

    Inv_SelectActiveList(0);

    i = 0x31;
    clear = D_800C0EAA;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0;
    category = 0x13;
    do {
        lookup = i;
        i++;
        src = Item_LookupBaseData(lookup);
    } while (src != 0 && src->kind != category);

    D_8009D03C = i;

    /* Nine grid cells cycle through the three records after the first
     * category match; each cell clears its third base stat and sets the
     * third bonus to the 999 placeholder. */
    i = 0;
    do {
        base = D_8009D03C + i % 3;
        src = Item_LookupBaseData(base - 1);
        D_800A1E64[i] = *src;
        D_800A1E6D[i][0] = 0;
        D_800A1E76[i][0] = 999;
    } while (++i < 9);

    i = 0x63;
    clear = D_800C1F7E;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    i = 0x51;
    clear = D_800C2022;
    do {
        *clear = 0;
        i--;
        clear--;
    } while (i >= 0);

    D_8009D04C = 0;
}

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
