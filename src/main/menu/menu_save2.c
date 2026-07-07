/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;

int g_MenuSaveBgFadeState;

void Menu_SaveBgStartFadeOut(void) {
    g_MenuSaveBgFadeState = 5;
    g_MenuSaveBgFadeStep = -1;
}

void Menu_SaveBgClearFade(void) {
    g_MenuSaveBgFadeState = 0;
}
