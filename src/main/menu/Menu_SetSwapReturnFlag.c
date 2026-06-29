/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuEquipSwapSource;
int g_MenuSwapReturnPending;

void Menu_SetSwapReturnFlag(void) {
    if (g_MenuEquipSwapSource != 0) {
        g_MenuSwapReturnPending = 1;
    }
}
