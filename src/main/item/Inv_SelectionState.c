#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)

extern u16 g_AyaInventoryItems[];
extern u16 *g_InvItemPtr;
extern u16 *g_InvActiveListOverride;
extern s32 g_InvSlotLimit;
extern s32 g_InvOverrideSlotLimit;
extern s32 *g_InvSelectionBits;
extern s32 g_InvSelectionBitWords;
extern s32 g_InvSavedSelectionIndex[3];
extern s32 g_InvSavedSelectionFromStorage[3];
extern s32 g_AyaItemSelectionBits[];
extern s32 g_InvStorageSelectionBits[];

int Inv_GetAyaSlotLimit(void);

void Inv_RememberSelection(unsigned int index, int value) {
    if (index < 2) {
        g_InvSavedSelectionIndex[index] = value;
        g_InvSavedSelectionFromStorage[index] = g_InvItemPtr != g_AyaInventoryItems;
    }
}

s32 Inv_RestoreSelection(u32 index) {
    if (index < 2U) {
        if ((g_InvSavedSelectionFromStorage[index] != 0) && (g_InvActiveListOverride != NULL)) {
            g_InvItemPtr = g_InvActiveListOverride;
            g_InvSelectionBits = &g_InvStorageSelectionBits;
            g_InvSelectionBitWords = 4;
            g_InvSlotLimit = g_InvOverrideSlotLimit;
        } else {
            g_InvItemPtr = &g_AyaInventoryItems;
            g_InvSlotLimit = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = &g_AyaItemSelectionBits;
            g_InvSelectionBitWords = 2;
        }
        return g_InvSavedSelectionIndex[index];
    }
    return -1;
}
