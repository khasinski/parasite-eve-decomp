/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawGradientBlendColor;

void Menu_SetSaveSlotBlendBase(int arg0) {
    g_DrawGradientBlendColor = arg0;
}
