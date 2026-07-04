/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

asm(".globl Draw_InitBuffers");
asm("Draw_InitBuffers = Draw_InitBuffers");

void SetDefDispEnv();
void SetDefDrawEnv();
void Draw_SetColor();
void Draw_SetFontVariant();

extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern void *g_TextCursorStackPtr;
extern s32 g_DrawGradientBlendColor;
extern s32 g_DrawPresentImage;
extern s8 D_800A2198[];
#define D_800A2198 (D_800A2198[0])
extern s8 D_800A2199[];
#define D_800A2199 (D_800A2199[0])
extern s8 D_800A219A[];
#define D_800A219A (D_800A219A[0])
extern s8 D_800A219B[];
#define D_800A219B (D_800A219B[0])
extern s32 g_DrawBufferOtBases[];
#define g_DrawBufferOtBases (g_DrawBufferOtBases[0])
extern u8 g_DrawBufferFrontBases[];
extern s8 D_800A2210[];
#define D_800A2210 (D_800A2210[0])
extern s8 D_800A2211[];
#define D_800A2211 (D_800A2211[0])
extern s8 D_800A2212[];
#define D_800A2212 (D_800A2212[0])
extern s8 D_800A2213[];
#define D_800A2213 (D_800A2213[0])
extern s32 D_800A2268[];
#define D_800A2268 (D_800A2268[0])
extern s32 D_800A226C[];
#define D_800A226C (D_800A226C[0])
extern M2C_UNK g_TextCursorStackBottom[];
#define g_TextCursorStackBottom (g_TextCursorStackBottom[0])
extern s32 g_OtBufferTable[];
#define g_OtBufferTable (g_OtBufferTable[0])
extern s32 g_RenderOtBufferBaseAlt[];
#define g_RenderOtBufferBaseAlt (g_RenderOtBufferBaseAlt[0])
extern s32 g_RenderFrontBufferBase[];
#define g_RenderFrontBufferBase (g_RenderFrontBufferBase[0])
extern s32 g_RenderBackBufferBase[];
#define g_RenderBackBufferBase (g_RenderBackBufferBase[0])

void Draw_InitBuffers(void);

void Draw_InitBuffers(void) {
    u8 *bufferBase = g_DrawBufferFrontBases;

    M2C_FIELD(bufferBase, s32 *, 0) = g_RenderFrontBufferBase;
    D_800A226C = g_RenderBackBufferBase;
    g_DrawBufferOtBases = g_OtBufferTable;
    D_800A2268 = g_RenderOtBufferBaseAlt;
    SetDefDrawEnv(bufferBase - 0x74, 0, 0, 0x140, 0xE0);
    SetDefDrawEnv(bufferBase + 4, 0, 0xE0, 0x140, 0xE0);
    D_800A2210 = 1;
    D_800A2198 = 1;
    D_800A2199 = 0;
    D_800A219A = 0;
    D_800A219B = 0;
    D_800A2211 = 0;
    D_800A2212 = 0;
    D_800A2213 = 0;
    SetDefDispEnv(bufferBase - 0x18, 0, 0xE0, 0x140, 0xE0);
    SetDefDispEnv(bufferBase + 0x60, 0, 0, 0x140, 0xE0);
    {
        s32 color;

        asm volatile(
            "lui $4, 0x80\n"
            "addiu $2, $0, 0x8\n"
            "sh $2, 0x6A($16)\n"
            "sh $2, -0xE($16)\n"
            "addiu $2, $0, 0xE0\n"
            "sh $2, 0x6E($16)\n"
            "sh $2, -0xA($16)"
            : "=r"(color)
            :
            : "v0", "memory");
        g_TextCursorY = 0;
        g_TextCursorX = 0;
        g_TextCursorStackPtr = &g_TextCursorStackBottom;
        Draw_SetColor(color | 0x8080);
    }
    g_DrawGradientBlendColor = 0;
    Draw_SetFontVariant(0);
    g_DrawPresentImage = 0;
}
