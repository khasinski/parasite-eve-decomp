/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* The equipped-slot bytes live at 0x800C0E20/0x800C0E22, outside the gp
 * window; incomplete array types keep -G8 from making them small data. */
extern void *g_InvItemPtr;
extern short g_AyaInventoryItems[];
extern signed char g_AyaEquippedWeaponSlot[];
extern signed char g_AyaEquippedArmorSlot[];

int Inv_IsAyaInventorySlotReserved(int arg0) {
    int result = 0;

    if (g_InvItemPtr == g_AyaInventoryItems) {
        if (g_AyaEquippedWeaponSlot[0] == arg0 || g_AyaEquippedArmorSlot[0] == arg0) {
            result = 1;
        }
    }

    return result;
}
