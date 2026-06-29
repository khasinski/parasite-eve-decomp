/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern volatile int g_TextCursorX;
extern volatile int g_TextCursorY;

void Draw_PrintNumberFixedWidth(int arg0, int arg1);

void Draw_PrintNumberWidth2(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 2);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 2;
    g_TextCursorY = other;
}
