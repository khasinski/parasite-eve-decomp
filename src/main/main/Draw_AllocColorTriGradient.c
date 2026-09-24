#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_DrawGradientBlendColor;
extern int g_DrawBlendColor;
extern struct { int value; char reserved[8]; } g_SavedDrawBlendColorObj __asm__("g_SavedDrawBlendColor");
extern int g_DrawScratchBuffer[];
extern u16 *g_DrawVertexWritePtr;
extern u16 g_TextCursorStackTop[];
extern u8 D_800930A8[];
extern int g_TextCursorX;
extern int g_TextCursorY;

void BoundsCheck_AssertStub(int arg0);
void Draw_EmitWipeBar(u8 *edges, int mode);
void Draw_AllocColorTri(int width, int height, int pulse);

#define DECLARE_BLUE_CURRENT register int blue asm("$5")
#define DECLARE_BLUE_SAVED register int blue asm("$6")

#define BLEND_HALF_CURRENT { \
    register int half asm("$5"); \
    register int sum asm("$2") = blue + (blendValue & 0xFF); \
    half = sum >> 1; \
    if (half < 0x100) { \
        register int shiftedHalf asm("$2") = half << 8; \
        value |= shiftedHalf; \
    } else value |= 0xFF00; \
}

#define BLEND_HALF_SAVED { \
    int half = (blue + (blendValue & 0xFF)) >> 1; \
    asm volatile("" : : "r"(blue), "r"(half)); \
    if (half < 0x100) value |= half << 8; \
    else value |= 0xFF00; \
}

#define BLEND_COLOR(color, blueDecl, halfBlock) do { \
    int blendValue = (color); \
    int *blendDst; \
    int value; \
    register int shifted asm("$2"); \
    blueDecl; \
    g_DrawBlendColor = blendValue & 0xFFFFFF; \
    shifted = blendValue >> 16; \
    blue = shifted & 0xFF; \
    value = (blue + ((blendValue >> 8) & 0xFF)) >> 1; \
    blendDst = g_DrawScratchBuffer; \
    if (value >= 0x100) value = 0xFF; \
    halfBlock \
    { \
        int half = (((blendValue >> 8) & 0xFF) + (blendValue & 0xFF)) >> 1; \
        *blendDst = value | ((half < 0x100) ? (half << 16) : 0xFF0000); \
    } \
} while (0)

#define PUSH_VERTEX_COND(xValue, yValue, condition) do { \
    int x = (xValue); \
    int y = (yValue); \
    u16 *out = g_DrawVertexWritePtr; \
    if (condition) { \
        out[1] = x; \
        g_DrawVertexWritePtr = out + 2; \
        out[0] = y; \
    } else { \
        BoundsCheck_AssertStub(4); \
    } \
} while (0)

#define PUSH_VERTEX(xValue, yValue) PUSH_VERTEX_COND(xValue, yValue, (u32)out < (u32)(g_TextCursorStackTop + 0x18))

void Draw_AllocColorTriGradient(int width, int height, int mode, int pulse) {
    int color = g_DrawGradientBlendColor;

    if (color != 0) {
        BLEND_COLOR(color, DECLARE_BLUE_CURRENT, BLEND_HALF_CURRENT);
    }

    {
        register u16 *vertexBase asm("$3") = g_TextCursorStackTop;
        u16 *vertexEnd = vertexBase + 0x18;
        int firstX = g_TextCursorX;
        int firstY = g_TextCursorY;
        register int firstFits asm("$2");
        asm volatile("" : : "r"(vertexEnd), "r"(firstX), "r"(firstY));
        firstFits = (u32)vertexBase < (u32)vertexEnd;
        g_DrawVertexWritePtr = vertexBase;
        PUSH_VERTEX_COND(firstX, firstY, firstFits);
    }
    PUSH_VERTEX(g_TextCursorX + width, g_TextCursorY);
    PUSH_VERTEX(g_TextCursorX, g_TextCursorY + height);
    PUSH_VERTEX(g_TextCursorX + width, g_TextCursorY + height);

    Draw_EmitWipeBar(D_800930A8, mode);
    g_TextCursorX -= 2;
    g_TextCursorY -= 2;
    Draw_AllocColorTri(width + 4, height + 4, pulse);

    if (g_DrawGradientBlendColor != 0) {
        BLEND_COLOR(g_SavedDrawBlendColorObj.value, DECLARE_BLUE_SAVED, BLEND_HALF_SAVED);
    }
}
