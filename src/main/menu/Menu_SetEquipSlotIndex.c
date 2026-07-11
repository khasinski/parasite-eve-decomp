/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern signed char D_8009CE94;

void Menu_SetEquipSlotIndex(int index) {
    D_8009CE94 = index;
}
