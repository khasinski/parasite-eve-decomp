/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawPrimColor;
int g_DrawColorShaded;

void Draw_SetColor(int value) {
    g_DrawPrimColor = value;
    g_DrawColorShaded = (value >> 1) & 0x7F7F7F;
}
