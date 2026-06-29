/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;

void Menu_SaveBgStartFadeOut(void) {
    g_MenuSaveBgFadeState = 5;
    g_MenuSaveBgFadeStep = -1;
}
