#include "pe1/draw_wipe_rect.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Draw_AllocColorTriGradient(int width, int height, int mode, int pulse)
{
    if (g_DrawGradientBlendColor != 0) {
        Draw_BlendColorInline(g_DrawGradientBlendColor);
    }

    g_DrawVertexWritePtr = (u16 *)g_TextCursorStackTop;
    DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY,
                          (u16 *)g_TextCursorStackTop + 0x18);
    DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY,
                          (u16 *)g_TextCursorStackTop + 0x18);
    DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY + height,
                          (u16 *)g_TextCursorStackTop + 0x18);
    DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY + height,
                          (u16 *)g_TextCursorStackTop + 0x18);

    Draw_EmitWipeBar(D_800930A8, mode);
    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(width + 4, height + 4, pulse);

    if (g_DrawGradientBlendColor != 0) {
        Draw_BlendColorInline(g_SavedDrawBlendColor[0]);
    }
}
