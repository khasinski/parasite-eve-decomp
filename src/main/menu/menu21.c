/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuEquipSwapSource;
int g_MenuSwapReturnPending;

int g_MenuSwapReturnPending;

void Menu_OpenInventoryScreen(void);

void Menu_SetSwapReturnFlag(void) {
    if (g_MenuEquipSwapSource != 0) {
        g_MenuSwapReturnPending = 1;
    }
}

void Menu_ProcessSwapReturnIfPending(void) {
    if (g_MenuSwapReturnPending != 0) {
        Menu_OpenInventoryScreen();
        g_MenuSwapReturnPending = 0;
    }
}
