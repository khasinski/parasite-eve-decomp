/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_TextCursorX;
int g_TextCursorY;

void Draw_OffsetCursor(int x, int y) {
    g_TextCursorX += x;
    g_TextCursorY += y;
}
