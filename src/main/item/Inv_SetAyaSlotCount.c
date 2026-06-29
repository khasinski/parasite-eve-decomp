/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

extern unsigned char g_AyaInventorySlotCount[];
int g_InvItemPtr;
int g_InvSlotLimit;

int Inv_GetAyaSlotLimit(void);

void Inv_SetAyaSlotCount(int count) {
    if (count >= 0x33) {
        count = 0x32;
    }

    g_AyaInventorySlotCount[0] = count;
    if (g_InvItemPtr == (int)(g_AyaInventorySlotCount + 0x3C)) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
}
