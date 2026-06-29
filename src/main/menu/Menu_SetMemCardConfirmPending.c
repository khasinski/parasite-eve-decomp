/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

unsigned int g_MenuMemCardConfirmPending;

void Menu_SetMemCardConfirmPending(void) {
    g_MenuMemCardConfirmPending = 1;
}
