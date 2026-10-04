#ifndef PE1_DRAW_LEVEL_BAR_H
#define PE1_DRAW_LEVEL_BAR_H

/* Declarations used by the stat bar renderer Draw_AllocTexturedRectAlt. */

#include "pe1/draw_state.h"
#include "pe1/render_prim.h"

/* A packet seen as the 32-bit address the ordering table links through. */
typedef union DrawLevelBarLink {
    RenderTexturedQuad *quad;
    RenderDrawModePacket *mode;
    u32 word;
} DrawLevelBarLink;

void BoundsCheck_AssertStub(int code);
void Draw_PrintNumberWidth2Unk(int value);
void Draw_AllocTexturedRectAlt(int value, int width);

#endif /* PE1_DRAW_LEVEL_BAR_H */
