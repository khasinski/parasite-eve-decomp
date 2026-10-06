/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/psyq_gpu.h"

extern int g_MenuInputActive;

int Draw_RemapStatusFlags(void);

int g_DrawPresentEnabled;

int MenuInput_GetStatusFlags(void) {
    if (g_MenuInputActive != 0) {
        return Draw_RemapStatusFlags();
    }
    return 0;
}

void Draw_SetPresentEnabled(int arg0) {
    g_DrawPresentEnabled = arg0;
}

#define NULL ((void *)0)

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
extern s32 g_DrawBufferFrontBases[];
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
extern s32 g_TextCursorStackBottom[];
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
    u8 *bufferBase = (u8 *)g_DrawBufferFrontBases;
    GpuDisplayBufferRecord *bufferRecord =
        (GpuDisplayBufferRecord *)(bufferBase -
                                   PE1_OFFSETOF(GpuDisplayBufferRecord,
                                                frontBufferBase));

    bufferRecord->frontBufferBase = g_RenderFrontBufferBase;
    D_800A226C = g_RenderBackBufferBase;
    g_DrawBufferOtBases[0] = g_OtBufferTable;
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
        register u8 *screenBase asm("$16") = bufferBase;

        /* Match debt: stop CSE rematerializing the four screen addresses.
         * A separate local keeps earlier buffer initialization unchanged. */
        asm volatile("" : "=r"(screenBase) : "0"(screenBase));
        color = 0x800000;
        ((DISPENV *)(screenBase + 0x60))->screen.y = 8;
        ((DISPENV *)(screenBase - 0x18))->screen.y = 8;
        ((DISPENV *)(screenBase + 0x60))->screen.h = 0xE0;
        ((DISPENV *)(screenBase - 0x18))->screen.h = 0xE0;
        g_TextCursorY = 0;
        g_TextCursorX = 0;
        g_TextCursorStackPtr = &g_TextCursorStackBottom;
        Draw_SetColor(color | 0x8080);
    }
    g_DrawGradientBlendColor = 0;
    Draw_SetFontVariant(0);
    g_DrawPresentImage = 0;
}

int g_DrawPresentImage;

extern int g_ActiveDrawSlot[];
extern int D_8009D0FC;
extern int g_ActiveDrawBuffer;
extern int g_DrawPacketBufferBase;
extern int g_DrawBufferIndex;
extern int D_8009D118;
extern int g_OtListTail;
extern int g_DrawPresentEnabled;
extern u8 D_800A2180[];

void ClearOTagR(int arg0, int arg1);

void Draw_SetPresentImage(int arg0) {
    g_DrawPresentImage = arg0;
}

void Draw_SelectBuffer(void) {
    int index;
    int offset;
    u8 *entry;
    int value0;
    int value1;

    if (g_DrawPresentEnabled != 0) {
        index = (u32)g_DrawBufferIndex < 1;
    } else {
        index = g_ActiveDrawSlot[0];
    }

    offset = ((index << 4) - index) << 3;
    g_DrawBufferIndex = index;
    entry = D_800A2180 + offset;
    value0 = *(int *)((u8 *)g_DrawBufferFrontBases + offset);
    value1 = *(int *)((u8 *)g_DrawBufferOtBases + offset);
    D_8009D0FC = (int)entry;
    g_DrawPacketBufferBase = value0;
    g_ActiveDrawBuffer = value0;
    D_8009D118 = value1;
    g_OtListTail = value1 + 4;

    if (g_DrawPresentEnabled != 0) {
        ClearOTagR(value1, 0x1000);
    }
}

extern int g_DrawPresentImage;

int VSync(int arg0);
void DrawSync(int arg0);
void ResetGraph(int arg0);
void PutDrawEnv(int arg0);
void LoadImage(s16 *rect, int image);
void DrawOTag(int arg0);

static inline int NormalizeSyncMode(int mode) {
    if (mode == 1) {
        mode = 0;
    }
    return mode;
}

void Draw_PresentFrame(int arg0) {
    int index;
    s16 rect[4];
    int mode;
    int image;
    int y;

    index = arg0;
    if (g_DrawPresentEnabled == 0) {
        return;
    }

    VSync(1);
    DrawSync(0);
    mode = index;
    index = 0xFFF;
    VSync(NormalizeSyncMode(mode));
    ResetGraph(1);
    PutDrawEnv(D_8009D0FC);
    PutDispEnv((DISPENV *)(D_8009D0FC + 0x5C));

    image = g_DrawPresentImage;
    if (image != 0) {
        y = 0xB;
        rect[0] = 0;
        if (g_DrawBufferIndex != 0) {
            y = 0xEB;
        }
        rect[1] = y;
        rect[2] = 0x140;
        rect[3] = 0xCC;
        LoadImage(rect, image);
    }

    {
        int offset;

        offset = index * 4;
        DrawOTag(D_8009D118 + offset);
    }
}
