/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_DrawPrimColor;
int g_DrawColorShaded;
int g_TextCursorX;
int g_TextCursorY;
int g_DrawTextPosX;
int D_8009D168;

void Draw_AllocColorTri(int arg0, int arg1, int arg2);

void Draw_DropShadow(void) {
    g_DrawPrimColor = 0xFF4000;
    g_DrawColorShaded = 0x7F2000;
    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(g_DrawTextPosX, D_8009D168, 0);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    g_TextCursorX += 2;
    g_TextCursorY += 2;
}
