/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

signed char g_MenuEquipSlotIndex;

int Menu_GetEquipSlotIndex(void) {
    return g_MenuEquipSlotIndex;
}
