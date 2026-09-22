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

extern unsigned char *g_DrawPacketCursor __asm__("D_8009D100");
extern unsigned char *g_DrawPacketArenaBase __asm__("D_8009D104");
extern int g_DrawColorSelect __asm__("D_8009D10C");
extern int g_DrawPrimaryColor __asm__("D_8009D110");
extern int g_DrawAlternateColor __asm__("D_8009D114");
extern unsigned int *g_DrawOrderingTableEntry __asm__("D_8009D11C");
extern int *g_TextCursorStack __asm__("D_8009D12C");
extern int g_DrawSpriteX __asm__("D_8009D124");
extern int g_DrawSpriteY __asm__("D_8009D128");

extern int g_TextCursorStackBottom[], g_TextCursorStackTop[];

void Draw_AllocColorTri(int width, int height, int mode);
void Draw_AllocSprite(int sprite);
void Draw_EmitDigitSprite(int digit);
extern int g_DrawDigitFontBaseTexU, g_DrawDigitFontBaseTexV;
extern int g_DrawDigitFontTpageClut;
void Draw_BlendColor(int color);
int Draw_GetBaseY(void);
void Draw_SetBaseOffsetPosition(int x, int y);

#endif /* PE1_DRAW_STATE_H */
