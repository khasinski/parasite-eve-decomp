/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawTextDimmed;

void Draw_SetTextDimmed(int value) {
    g_DrawTextDimmed = value;
}
