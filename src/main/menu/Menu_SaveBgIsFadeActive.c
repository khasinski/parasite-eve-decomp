/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuSaveBgFadeState;

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}
