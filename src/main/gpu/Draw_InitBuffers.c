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
asm("Draw_InitBuffers = func_8005E588");

void SetDefDispEnv();
void SetDefDrawEnv();
void Draw_SetColor() __asm__("func_8005E968");
void Draw_SetFontVariant() __asm__("func_8005F844");

extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern void *g_TextCursorStackPtr;
extern s32 g_DrawGradientBlendColor;
extern s32 g_DrawPresentImage;
extern struct {
    char _[16];
} D_800A2198_o __asm__("D_800A2198");
#define D_800A2198 (*(s8 *)&D_800A2198_o)
extern struct {
    char _[16];
} D_800A2199_o __asm__("D_800A2199");
#define D_800A2199 (*(s8 *)&D_800A2199_o)
extern struct {
    char _[16];
} D_800A219A_o __asm__("D_800A219A");
#define D_800A219A (*(s8 *)&D_800A219A_o)
extern struct {
    char _[16];
} D_800A219B_o __asm__("D_800A219B");
#define D_800A219B (*(s8 *)&D_800A219B_o)
extern struct {
    char _[16];
} D_800A21F0_o __asm__("g_DrawBufferOtBases");
#define g_DrawBufferOtBases (*(s32 *)&D_800A21F0_o)
extern struct {
    char _[16];
} D_800A21F4_o __asm__("g_DrawBufferFrontBases");
#define g_DrawBufferFrontBases ((u8 *)&D_800A21F4_o)
extern struct {
    char _[16];
} D_800A2210_o __asm__("D_800A2210");
#define D_800A2210 (*(s8 *)&D_800A2210_o)
extern struct {
    char _[16];
} D_800A2211_o __asm__("D_800A2211");
#define D_800A2211 (*(s8 *)&D_800A2211_o)
extern struct {
    char _[16];
} D_800A2212_o __asm__("D_800A2212");
#define D_800A2212 (*(s8 *)&D_800A2212_o)
extern struct {
    char _[16];
} D_800A2213_o __asm__("D_800A2213");
#define D_800A2213 (*(s8 *)&D_800A2213_o)
extern struct {
    char _[16];
} D_800A2268_o __asm__("D_800A2268");
#define D_800A2268 (*(s32 *)&D_800A2268_o)
extern struct {
    char _[16];
} D_800A226C_o __asm__("D_800A226C");
#define D_800A226C (*(s32 *)&D_800A226C_o)
extern struct {
    char _[16];
} D_800A2270_o __asm__("g_TextCursorStackBottom");
#define g_TextCursorStackBottom (*(M2C_UNK *)&D_800A2270_o)
extern struct {
    char _[16];
} D_800B0E38_o __asm__("g_OtBufferTable");
#define g_OtBufferTable (*(s32 *)&D_800B0E38_o)
extern struct {
    char _[16];
} D_800B0E3C_o __asm__("g_RenderOtBufferBaseAlt");
#define g_RenderOtBufferBaseAlt (*(s32 *)&D_800B0E3C_o)
extern struct {
    char _[16];
} D_800B0E50_o __asm__("g_RenderFrontBufferBase");
#define g_RenderFrontBufferBase (*(s32 *)&D_800B0E50_o)
extern struct {
    char _[16];
} D_800B0E54_o __asm__("g_RenderBackBufferBase");
#define g_RenderBackBufferBase (*(s32 *)&D_800B0E54_o)

void Draw_InitBuffers(void) __asm__("func_8005E588");

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
