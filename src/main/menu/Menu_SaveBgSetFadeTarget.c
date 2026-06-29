/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeTint;

void Menu_SaveBgSetFadeTarget(int arg0) {
    g_MenuSaveBgFadeTint = arg0;
}
