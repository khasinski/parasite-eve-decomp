/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_TextCursorX;
extern int g_TextCursorY;
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
