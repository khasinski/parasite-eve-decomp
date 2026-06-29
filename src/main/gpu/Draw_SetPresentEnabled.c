/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawPresentEnabled;

void Draw_SetPresentEnabled(int arg0) {
    g_DrawPresentEnabled = arg0;
}
