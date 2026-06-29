/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;

void Menu_SaveBgClearFade(void) {
    g_MenuSaveBgFadeState = 0;
}
