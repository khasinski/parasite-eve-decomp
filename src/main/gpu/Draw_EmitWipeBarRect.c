/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "common.h"

extern int g_TextCursorX;
extern int g_TextCursorY;
extern u16 g_TextCursorStackTop[];
extern u16 *g_DrawVertexWritePtr;
extern u8 D_800930A8[];

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *arg0, int arg1);

#define PUSH_WIPE_BAR_VERTEX(xValue, yValue)                     \
    {                                                           \
        int x = (xValue);                                       \
        int y = (yValue);                                       \
        u16 *out = g_DrawVertexWritePtr;                         \
                                                                \
        if ((unsigned int)out <                                  \
            (unsigned int)(g_TextCursorStackTop + 0x18)) {       \
            out[1] = x;                                          \
            g_DrawVertexWritePtr = out + 2;                      \
            out[0] = y;                                          \
        } else {                                                 \
            BoundsCheck_AssertStub(4);                           \
        }                                                       \
    }

void Draw_EmitWipeBarRect(int width, int height, int mode)
{
    g_DrawVertexWritePtr = g_TextCursorStackTop;

    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY + height);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY + height);

    Draw_EmitWipeBar(D_800930A8, mode);
}
