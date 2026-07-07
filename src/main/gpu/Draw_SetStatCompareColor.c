/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

extern unsigned int g_DrawPrimColor;
extern unsigned int g_DrawColorShaded;

void Draw_SetStatCompareColor(int arg0, int arg1) {
    unsigned int color;

    if (arg0 >= arg1) {
        color = 0x808080;
        if (arg1 < arg0) {
            color = 0x404080;
        }
    } else {
        color = 0x408080;
    }

    g_DrawPrimColor = color;
    g_DrawColorShaded = color >> 1;
}
