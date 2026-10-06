/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

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

extern int *g_TextCursorStackPtr;
extern int g_TextCursorStackTop[];

void BoundsCheck_AssertStub(int arg0);

void Draw_StatePush(void) {
    int *cursor;
    int x;
    int y;

    cursor = g_TextCursorStackPtr;
    if (cursor < g_TextCursorStackTop) {
        x = g_TextCursorX;
        y = g_TextCursorY;
        g_TextCursorStackPtr = cursor + 2;
        cursor[0] = x;
        cursor[1] = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

extern int g_TextCursorStackBottom[];

void Draw_StatePop(void) {
    int *cursor;
    int x;
    int y;

    cursor = g_TextCursorStackPtr;
    if (g_TextCursorStackBottom < cursor) {
        x = cursor[-2];
        y = cursor[-1];
        g_TextCursorStackPtr = cursor - 2;
        g_TextCursorX = x;
        g_TextCursorY = y;
    } else {
        BoundsCheck_AssertStub(3);
    }
}

unsigned int g_DrawPrimColor;
unsigned int g_DrawColorShaded;

void Draw_SetColor(int value) {
    g_DrawPrimColor = value;
    g_DrawColorShaded = (value >> 1) & 0x7F7F7F;
}

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
