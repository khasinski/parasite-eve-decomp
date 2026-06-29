/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_TextCursorX;
int g_TextCursorY;

void Draw_SetCursor(int arg0, int arg1) {
    g_TextCursorX = arg0;
    g_TextCursorY = arg1;
}
