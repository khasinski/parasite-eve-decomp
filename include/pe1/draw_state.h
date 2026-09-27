#ifndef PE1_DRAW_STATE_H
#define PE1_DRAW_STATE_H

#include "common.h"

/* Lookup entries have an eight-byte stride; the final byte is not read here. */
typedef struct DrawGlyphDescriptor {
    u8 u, v;
    u16 clut;
    u8 width, height, mode, reserved;
} DrawGlyphDescriptor;

PE1_STATIC_ASSERT(sizeof(DrawGlyphDescriptor) == 8, draw_glyph_descriptor_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DrawGlyphDescriptor, mode) == 6, draw_glyph_mode_offset);
void *Draw_LookupGlyphDescriptor(int index);

/* Semantic C names for the existing, independently addressed linker objects. */
extern unsigned char *D_8009D100;
extern unsigned char *D_8009D104;
extern int D_8009D108;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern unsigned int *D_8009D11C;
extern int *D_8009D12C;
extern int D_8009D124;
extern int D_8009D128;

#define g_DrawPacketCursor D_8009D100
#define g_DrawPacketArenaBase D_8009D104
#define g_DrawColorSelect D_8009D10C
#define g_DrawPrimaryColor D_8009D110
#define g_DrawAlternateColor D_8009D114
#define g_DrawOrderingTableEntry D_8009D11C
#define g_TextCursorStack D_8009D12C
#define g_DrawSpriteX D_8009D124
#define g_DrawSpriteY D_8009D128

extern int g_TextCursorStackBottom[], g_TextCursorStackTop[];

void Draw_AllocColorTri(int width, int height, int mode);
void Draw_AllocColorRect(int firstVertex, int secondVertex, int width, int mode);
void Draw_EmitWipeBar(u8 *edges, int mode);
void Draw_AllocSprite(int sprite);
void Draw_EmitDigitSprite(int digit);
void Draw_AllocTexturedQuad(int glyph);
int Draw_LookupGlyphMetrics(int glyph);
void Draw_PrintTextWrapped(u8 *text, int width);
void Draw_PrintCenteredTextInWidth(u8 *text, int width);
extern int g_TextRenderMode, g_DrawGlyphAdvance;
extern int g_DrawDigitFontBaseTexU, g_DrawDigitFontBaseTexV;
extern int g_DrawDigitFontTpageClut;
void Draw_BlendColor(int color);
int Draw_GetBaseY(void);
void Draw_SetBaseOffsetPosition(int x, int y);

void Draw_PrintNumberWidth4Unk(int value);
void Draw_PrintSignedNumberWidth4(int value);

void Draw_SetStatCompareColor(int current, int candidate);
void Draw_PrintNumberWidth4(int value);
void Draw_SetColor(int color);

#endif /* PE1_DRAW_STATE_H */
