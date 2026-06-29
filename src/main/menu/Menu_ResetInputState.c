/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned int g_SavedMenuMode;

extern int Menu_GetActiveMode(void);
extern void Window_SetBoundsByMode(int mode);

void Menu_ResetInputState(void) {
    g_SavedMenuMode = Menu_GetActiveMode() & 0xFF;
    Window_SetBoundsByMode(0);
}
