/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawPresentImage;

void Draw_SetPresentImage(int arg0) {
    g_DrawPresentImage = arg0;
}
