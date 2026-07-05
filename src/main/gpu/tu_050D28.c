/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern volatile int g_TextCursorX;
extern volatile int g_TextCursorY;

void Draw_PrintNumberFixedWidth(int arg0, int arg1);

void Draw_PrintNumberWidth6(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 6);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 6;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth5(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 5);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 5;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth4(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 4);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 4;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth3(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 3);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 3;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth2(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 2);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 2;
    g_TextCursorY = other;
}
