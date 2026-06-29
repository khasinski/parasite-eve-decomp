/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawBlendColor;

int Draw_GetBlendColor(void) {
    return g_DrawBlendColor;
}
