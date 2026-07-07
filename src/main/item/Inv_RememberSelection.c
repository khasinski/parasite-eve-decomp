/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned short u16;

extern u16 g_AyaInventoryItems[];
extern u16 *g_InvItemPtr;
extern int g_InvSavedSelectionIndex[3];
extern int g_InvSavedSelectionFromStorage[3];

void Inv_RememberSelection(unsigned int index, int value) {
    if (index < 2) {
        g_InvSavedSelectionIndex[index] = value;
        g_InvSavedSelectionFromStorage[index] = g_InvItemPtr != g_AyaInventoryItems;
    }
}
