/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSwapReturnPending;

void Menu_OpenInventoryScreen(void);

void Menu_ProcessSwapReturnIfPending(void) {
    if (g_MenuSwapReturnPending != 0) {
        Menu_OpenInventoryScreen();
        g_MenuSwapReturnPending = 0;
    }
}
