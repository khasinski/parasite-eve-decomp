/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_TextCursorX;
extern int g_TextCursorY;
extern int *g_TextCursorStackPtr;
extern int g_TextCursorStackBottom;

void BoundsCheck_AssertStub(int arg0);

void Draw_StatePop(void) {
    int *cursor;
    int x;
    int y;

    cursor = g_TextCursorStackPtr;
    if (&g_TextCursorStackBottom < cursor) {
        x = cursor[-2];
        y = cursor[-1];
        g_TextCursorStackPtr = cursor - 2;
        g_TextCursorX = x;
        g_TextCursorY = y;
    } else {
        BoundsCheck_AssertStub(3);
    }
}
