/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;

int Menu_SaveBgIsFadeComplete(void) {
    return g_MenuSaveBgFadeState == 7;
}
