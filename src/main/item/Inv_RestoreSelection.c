/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)

extern void *g_InvItemPtr;
extern void *g_InvActiveListOverride;
extern s32 g_InvSlotLimit;
extern s32 g_InvOverrideSlotLimit;
extern void *g_InvSelectionBits;
extern s32 g_InvSelectionBitWords;
extern s32 g_InvSavedSelectionIndex[];
extern s32 g_InvSavedSelectionFromStorage[];
extern s32 g_AyaItemSelectionBits[];
extern s32 g_InvStorageSelectionBits[];
extern s16 g_AyaInventoryItems[];

int Inv_GetAyaSlotLimit(void);

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
