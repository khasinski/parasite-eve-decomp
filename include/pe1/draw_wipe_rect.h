#ifndef PE1_DRAW_WIPE_RECT_H
#define PE1_DRAW_WIPE_RECT_H

#include "common.h"

/* Rectangular wipe bars: four corner vertices pushed onto the halfword
 * vertex stack, then the shared wipe-bar edge list. The blended palette
 * is the inline form of Draw_BlendColor. */

extern int g_DrawGradientBlendColor;
extern int g_DrawBlendColor;
extern int g_SavedDrawBlendColor[];
extern int g_DrawScratchBuffer[];
extern u16 *g_DrawVertexWritePtr;
extern u16 g_TextCursorStackTop[];
extern u8 D_800930A8[];
extern int g_TextCursorX;
extern int g_TextCursorY;

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *edges, int mode);
void Draw_AllocColorTri(int width, int height, int pulse);

static inline void Draw_BlendColorInline(int color)
{
    int *dst;
    int value;

    g_DrawBlendColor = color & 0xFFFFFF;

    value = (((color >> 16) & 0xFF) + ((color >> 8) & 0xFF)) >> 1;
    dst = g_DrawScratchBuffer;
    if (value >= 0x100) {
        value = 0xFF;
    }

    {
        int half = (((color >> 16) & 0xFF) + (color & 0xFF)) >> 1;
        if (half < 0x100) {
            value |= half << 8;
        } else {
            value |= 0xFF00;
        }
    }
    {
        int half = (((color >> 8) & 0xFF) + (color & 0xFF)) >> 1;
        *dst = value | ((half < 0x100) ? (half << 16) : 0xFF0000);
    }
}

/* Push one (x, y) vertex, stored y first, while below `limit`. */
#define DRAW_PUSH_WIPE_VERTEX(xValue, yValue, limit) \
    {                                                \
        int x = (xValue);                            \
        int y = (yValue);                            \
        u16 *out = g_DrawVertexWritePtr;             \
                                                     \
        if (out < (limit)) {                         \
            out[1] = x;                              \
            g_DrawVertexWritePtr = out + 2;          \
            out[0] = y;                              \
        } else {                                     \
            BoundsCheck_AssertStub(4);               \
        }                                            \
    }

#endif /* PE1_DRAW_WIPE_RECT_H */
