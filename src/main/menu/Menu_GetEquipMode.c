/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuEquipMode;

int Menu_GetEquipMode(void) {
    return g_MenuEquipMode;
}
