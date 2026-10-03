#ifndef MENU_MEMCARD_SCREEN_H
#define MENU_MEMCARD_SCREEN_H

#include "common.h"
#include "pe1/psyq_gpu.h"

/* One of the two full-screen buffers the memory card screens flip between. */
typedef struct MemcardScreenBuffer {
    DRAWENV draw;      /* 0x00 */
    DISPENV disp;      /* 0x5C */
    RECT overlay;      /* 0x70 area refreshed from overlayImage */
    RECT dirty;        /* 0x78 area the image nodes covered last frame */
    u8 pad80[0x8000];
    u8 overlayImage[1]; /* 0x8080 */
} MemcardScreenBuffer;

typedef struct MemcardSprite {
    u8 addr[3];
    u8 len;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    s16 w, h;
} MemcardSprite;

typedef struct MemcardDrawTPage {
    u32 tag;
    u32 code;
} MemcardDrawTPage;

/* A TIM image block: byte size, VRAM rectangle, pixels. */
typedef struct MemcardTimBlock {
    u32 size;
    RECT rect;
    u32 data[1];
} MemcardTimBlock;

typedef struct MemcardTim {
    u32 id;
    u32 flags;
    MemcardTimBlock clut;
} MemcardTim;

extern MemcardScreenBuffer *D_801D11BC[2];
extern MemcardScreenBuffer *D_801D11C4;
extern s32 D_801D11C8;
extern u8 D_80193254[];
extern s32 D_80193278;

void func_8007506C(RECT *rect, void *data); /* LoadImage */
void func_80077C84(MemcardDrawTPage *packet, s32 dfe, s32 dtd, s32 tpage); /* SetDrawTPage */
void func_80075358(void *packet); /* DrawPrim */
void func_800755F0(DISPENV *env); /* PutDispEnv */
void PutDrawEnv(DRAWENV *env);
void DrawSync(s32 mode);
s32 VSync(s32 mode);
void SetDispMask(s32 mask);
void func_80074A44(s32 mode);

#endif
