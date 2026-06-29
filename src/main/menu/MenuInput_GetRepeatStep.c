/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuDrawPhase;

int MenuInput_GetRepeatStep(void) {
    return g_MenuDrawPhase;
}
