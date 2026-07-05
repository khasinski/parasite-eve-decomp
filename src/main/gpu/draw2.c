/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_TextCursorX;
int g_TextCursorY;

void Draw_SetCursor(int arg0, int arg1) {
    g_TextCursorX = arg0;
    g_TextCursorY = arg1;
}

void Draw_OffsetCursor(int x, int y) {
    g_TextCursorX += x;
    g_TextCursorY += y;
}
