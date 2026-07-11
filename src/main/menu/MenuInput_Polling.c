/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

extern int D_8009D0EC;
extern int D_8009D0F4;

void MenuInput_SetPollingPaused(int paused) {
    D_8009D0EC = paused;
}

int MenuInput_GetRepeatStep(void) {
    return D_8009D0F4;
}
