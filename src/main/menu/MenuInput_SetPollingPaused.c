/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuInputPollingPaused;

void MenuInput_SetPollingPaused(int arg0) {
    g_MenuInputPollingPaused = arg0;
}
