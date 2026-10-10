/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/draw_state.h"

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

#include "pe1/bounds_check.h"

void Draw_StatePush(void) {
    DrawTextCursorPair *cursor;
    int x;
    int y;

    cursor = (DrawTextCursorPair *)g_TextCursorStackPtr;
    if (cursor < (DrawTextCursorPair *)g_TextCursorStackTop) {
        x = g_TextCursorX;
        y = g_TextCursorY;
        g_TextCursorStackPtr = (int *)(cursor + 1);
        cursor->x = x;
        cursor->y = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

extern int g_TextCursorStackBottom[];

void Draw_StatePop(void) {
    DrawTextCursorPair *cursor;
    int x;
    int y;

    cursor = (DrawTextCursorPair *)g_TextCursorStackPtr;
    if ((DrawTextCursorPair *)g_TextCursorStackBottom < cursor) {
        x = cursor[-1].x;
        y = cursor[-1].y;
        g_TextCursorStackPtr = (int *)(cursor - 1);
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
