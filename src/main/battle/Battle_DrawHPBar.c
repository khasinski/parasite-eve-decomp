/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "common.h"
#include "pe1/render_prim.h"
#include "pe1/draw_state.h"
void Gpu_InitDrawModeSprtPacket(void *, int);
void Gpu_InitDrawModeTilePacket(void *, int);
void Gpu_SetDither(void *, int);
void Gpu_SetDrawEnable(void *, int);
void SetPolyF3(void *);
void SetPolyG4(void *);
void SetPolyFT4(RenderTexturedQuad *);
void SetTile(void *);
u16 GetClut(int, int);

typedef struct {
    s8 values[3];
} HudShades;

extern u8 D_8009CD90[];
extern u8 D_8009E068[];
extern u8 D_8009E098[];
extern u8 D_8009E0B8[];
extern u8 D_8009E0CE[];
extern u8 D_8009E0F0[];
extern u8 D_8009E106[];
extern u8 D_8009E1D0[];
extern u8 D_8009E1E6[];
extern u8 D_8009E2E8[];
extern u8 D_8009E2FE[];
extern u8 D_8009E320[];
extern u8 D_8009E336[];
extern u8 D_8009E358[];
extern u8 D_8009E3B8[];
extern u8 D_8009E3CE[];
extern u8 D_8009E460[];
extern u8 D_8009E476[];
extern u8 D_8009E498[];
extern u8 D_8009E4D8[];
extern u8 D_8009E500[];
extern u8 D_8009E730[];
extern u8 D_8009E746[];
extern u8 D_8009E768[];
extern u8 D_8009E77E[];
extern u8 D_8009E7A0[];
extern u8 D_8009E7B6[];
extern u8 D_8009E880[];
extern u8 D_8009E896[];
extern u8 D_8009E8B8[];
extern u8 D_8009E8CE[];
extern u8 D_8009E928[];
extern u8 D_8009E93E[];
extern u8 D_8009E960[];
extern u8 D_8009E976[];
extern u8 D_8009EC38[];
extern u8 D_800B00E8[];
extern u8 D_800B0130[];
typedef struct {
    RenderTexturePagePacket page;
    RenderSpritePacket sprite;
} HudSprite;
extern HudSprite D_800B01C0[2][10][5];
extern u8 D_800B6920[];
extern u8 D_800B6936[];
extern u8 D_800BE9F0[];
extern u8 D_800BE9FE[];
extern u8 D_800BEA06[];

/* Initialize the HUD packet pools for both draw buffers. The legacy symbol
 * name is retained; this routine sets up more than the HP bar.
 * Register constraints and address views are recorded in ASM_AND_GTE_POLICY.md.
 */
void Battle_DrawHPBar(void) {
    HudShades shades;
    s32 i;
    s32 j;
    u32 base0;
    u32 body0;
    u32 base1;
    u32 body1;
    u32 base2;
    u32 body2;
    u32 base3;
    u32 body3;
    u32 base4;
    u32 body4;
    u32 base5;
    u32 body5;
    u16 pageCode;
    u32 glyphBase;
    u32 body6;
    u32 tileBase;
    u32 tileIndex;
    s32 highBlue;

    u8 glyphHeight;
    register u8 markerV asm("$18");
    u8 bufferIndex;
    u16 hudClut;

    RenderLinePacket *temp_s2_5;
    RenderLinePacket *temp_s4_2;
    RenderTexturedQuad *labelQuad;
    register s32 hudPage;
    s32 temp_s0_10;
    s32 temp_s0_13;
    s32 temp_s0_16;
    s32 temp_s0_17;
    s32 temp_s0_19;
    s32 temp_s0_22;
    s32 temp_s0_23;
    s32 temp_s0_26;
    s32 temp_s0_27;
    s32 temp_s0_29;

    s32 temp_s0_31;
    s32 temp_s0_32;
    s32 temp_s0_33;
    s32 temp_s0_3;
    s32 temp_s0_6;
    s32 temp_s0_7;
    s32 temp_s0_9;
    s32 temp_s1;
    s32 temp_s1_10;
    s32 temp_s1_11;

    s32 temp_s1_5;
    s32 temp_s1_8;
    s32 temp_s1_9;
    s32 temp_s2;
    s32 temp_s2_2;
    s32 temp_s2_4;
    s32 temp_s2_6;
    s32 temp_s2_7;
    s32 temp_s3_2;
    s32 temp_s3_4;
    s32 temp_s3_5;
    s32 temp_s4;
    s32 temp_s4_3;

    u8 temp_v0;
    RenderSpritePacket *temp_s0_11;
    RenderSpritePacket *temp_s0_12;
    RenderColorTilePacket *temp_s0_14;
    RenderSpritePacket *temp_s0_15;
    RenderSpritePacket *temp_s0_18;
    RenderSpritePacket *temp_s0_20;
    RenderSpritePacket *temp_s0_21;
    RenderSpritePacket *temp_s0_24;
    RenderSpritePacket *temp_s0_25;
    RenderSpritePacket *temp_s0_28;
    RenderSpritePacket *temp_s0_30;
    u8 gridRow;
    RenderSpritePacket *temp_s0_34;
    RenderColorTilePacket *temp_s0_4;
    RenderGouraudQuad *primaryGradient;
    RenderSpritePacket *temp_s0_8;
    RenderSpritePacket *temp_s1_12;
    RenderColorTilePacket *temp_s1_3;
    RenderSpritePacket *temp_s1_4;
    RenderGouraudQuad *greenGradient;
    RenderSpritePacket *temp_s1_7;
    RenderGouraudQuad *pinkGradient;
    register DrawGlyphDescriptor *labelGlyph asm("$19");
    RenderSpritePacket *temp_s3_3;
    register DrawGlyphDescriptor *digitGlyph asm("$19");
    RenderSpritePacket *temp_v1;

    shades = *(const HudShades *)D_8009CD90;
    hudPage = GetTPage(0, 1, 0x100, 0x1E0);
    hudClut = GetClut(0x130, 0x1F8);
    bufferIndex = 0;
    do {
        labelGlyph = Draw_LookupGlyphDescriptor(0x8B);
        asm("" : "=r"(labelGlyph) : "0"(labelGlyph));
        temp_s1 = bufferIndex * 0x28;
        labelQuad = temp_s1 + D_800BE9F0;
        SetPolyFT4(labelQuad);
        {
            register s32 pa0 asm("$4");
            register s32 pa1 asm("$5");
            register s32 pa2 asm("$6");
            register s32 pa3 asm("$7");
            labelQuad->u0 = labelGlyph->u;
            labelQuad->v0 = labelGlyph->v;
            labelQuad->u1 = labelGlyph->u + labelGlyph->width;
            labelQuad->v1 = labelGlyph->v;
            labelQuad->u2 = labelGlyph->u;
            labelQuad->v2 = labelGlyph->v + labelGlyph->height;
            labelQuad->u3 = labelGlyph->u + labelGlyph->width;
            pa0 = 0;
            pa1 = 0;
            pa2 = 0x1C0;
            pa3 = 0;
            asm volatile("" :  : "r"(pa0), "r"(pa1), "r"(pa2), "r"(pa3) : "$22");
            i = 0;
            labelQuad->v3 = labelGlyph->v + labelGlyph->height;
            *(u16 *)(D_800BEA06 + temp_s1) = GetTPage(pa0, pa1, pa2, pa3);
        }
        *(u16 *)(D_800BE9FE + temp_s1) = labelGlyph->clut;
        labelQuad->x0 = 0;
        highBlue = 0x82;

        labelQuad->y0 = 0;
        labelQuad->x1 = 0;
        labelQuad->y1 = 0;
        labelQuad->x2 = 0;
        labelQuad->y2 = 0;
        labelQuad->x3 = 0;
        labelQuad->y3 = 0;
        labelQuad->color.bytes.r = 0;
        labelQuad->color.bytes.g = 0;
        labelQuad->color.bytes.b = 0;
        Gpu_SetDither(labelQuad, 1);
        {
            s32 j;
            u8 buffer;
            buffer = bufferIndex;

            for (i = 0; (u8)i < 10; i++) {
                gridRow = (u8)i;
                for (j = 0; (u8)j < 4; j++) {
                    asm("" : : "r"(hudPage));
                    Gpu_InitDrawModeSprtPacket(&D_800B01C0[buffer][gridRow][(u8)j], hudPage);
                    D_800B01C0[buffer][gridRow][(u8)j].sprite.clut = hudClut;
                }
            }
        }
        pageCode = GetTPage(0, 0, 0, 0);
        temp_s0_3 = bufferIndex * 0x18;
        Gpu_InitDrawModeTilePacket(temp_s0_3 + D_8009E068, pageCode);
        temp_s0_4 = temp_s0_3 + (D_8009E068 + 8);
        temp_s0_4->r0 = 0x30;
        temp_s0_4->g0 = 0x30;
        temp_s0_4->b0 = 0x30;
        Gpu_SetDither(temp_s0_4, 1);
        temp_s1_3 = (bufferIndex * 0x10) + D_8009E098;
        SetTile(temp_s1_3);
        primaryGradient = (bufferIndex * 0x24) + D_800B00E8;
        temp_s1_3->r0 = 0x1D;
        temp_s1_3->g0 = 0x3E;
        temp_s1_3->b0 = 0x32;
        temp_s1_3->w = 0x38;
        temp_s1_3->h = 3;
        SetPolyG4(primaryGradient);
        temp_s2 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s2 + D_800B6920, hudPage);
        i = 0;
        temp_s3_2 = bufferIndex * 0x70;
        base0 = (u32)D_8009E0F0;
        body0 = base0 + 8;
        temp_s1_4 = temp_s2 + (D_800B6920 + 8);
        temp_s1_4->u = 0xC8;
        temp_s1_4->v = 0xE0;
        *(u16 *)(D_800B6936 + temp_s2) = hudClut;
        temp_s1_4->width = 4;
        temp_s1_4->height = 8;
        primaryGradient->b0 = highBlue;
        primaryGradient->g1 = 0xFF;
        primaryGradient->r0 = 0;
        primaryGradient->g0 = 0x46;
        primaryGradient->r1 = 0x9F;
        primaryGradient->b2 = highBlue;
        primaryGradient->b1 = 0xF9;
        primaryGradient->r2 = 0;
        primaryGradient->g2 = 0x46;
        primaryGradient->r3 = 0x9F;
        primaryGradient->g3 = 0xFF;
        primaryGradient->b3 = 0xF9;
        temp_s1_4->color.bytes.r = 0x9F;
        temp_s1_4->color.bytes.g = 0xFF;
        temp_s1_4->color.bytes.b = 0xF9;

loop_6:
        temp_s0_6 = ((u8)i) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s3_2 + (temp_s0_6 + base0), hudPage);
        i += 1;
        temp_s0_7 = temp_s0_6 + temp_s3_2;
        *(u16 *)(D_8009E106 + temp_s0_7) = hudClut;
        temp_s0_8 = temp_s0_7 + body0;
        temp_s0_8->width = 6;
        temp_s0_8->height = 0xA;

        if ((u32) (i & 0xFF) < 4U) {
            goto loop_6;
        }
        i = 0;
        temp_s1_5 = bufferIndex * 0x8C;
        base1 = (u32)D_8009E1D0;
        body1 = base1 + 8;

loop_8:
        temp_s0_9 = ((u8)i) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s1_5 + (temp_s0_9 + base1), hudPage);
        i += 1;
        temp_s0_10 = temp_s0_9 + temp_s1_5;
        *(u16 *)(D_8009E1E6 + temp_s0_10) = hudClut;
        temp_s0_11 = temp_s0_10 + body1;
        temp_s0_11->width = 6;
        temp_s0_11->height = 0xA;

        if ((u32) (i & 0xFF) < 5U) {
            goto loop_8;
        }
        temp_s2_2 = bufferIndex * 0x48;
        greenGradient = temp_s2_2 + D_800B0130;
        SetPolyG4(greenGradient);
        pinkGradient = temp_s2_2 + (D_800B0130 + 0x24);
        SetPolyG4(pinkGradient);
        temp_s4 = bufferIndex * 0x1C;
        {
            u8 red;
            u8 blue;
            red = 0x4A;
            blue = 0x3B;
            greenGradient->g0 = highBlue;
            greenGradient->r0 = 0;
            greenGradient->b0 = 0x36;
            greenGradient->r1 = red;
            greenGradient->g1 = 0xFF;
            greenGradient->b1 = blue;
            greenGradient->g2 = highBlue;
            greenGradient->r2 = 0;
            greenGradient->b2 = 0x36;
            greenGradient->r3 = red;
            greenGradient->g3 = 0xFF;
            greenGradient->b3 = blue;
        }
        pinkGradient->r0 = 0xFF;
        pinkGradient->g0 = 0x3D;
        pinkGradient->b0 = 0x81;
        pinkGradient->r1 = 0x83;
        pinkGradient->g1 = 0x13;
        pinkGradient->b1 = 1;
        pinkGradient->r2 = 0xFF;
        pinkGradient->g2 = 0x3D;
        pinkGradient->b2 = 0x81;
        pinkGradient->r3 = 0x83;
        pinkGradient->g3 = 0x13;
        pinkGradient->b3 = 1;
        Gpu_InitDrawModeSprtPacket(temp_s4 + D_8009E0B8, hudPage);
        temp_s3_3 = temp_s4 + (D_8009E0B8 + 8);
        Gpu_SetDrawEnable(temp_s3_3, 1);
        markerV = 0xF4;
        temp_s3_3->u = 0x50;
        temp_s3_3->v = markerV;
        temp_s3_3->color.bytes.r = 0x80;
        temp_s3_3->color.bytes.g = 0x80;
        temp_s3_3->color.bytes.b = 0x80;
        *(u16 *)(D_8009E0CE + temp_s4) = hudClut;
        temp_s3_3->width = 8;
        temp_s3_3->height = 4;
        Gpu_InitDrawModeSprtPacket(temp_s4 + D_8009E2E8, hudPage);
        temp_s0_12 = temp_s4 + (D_8009E2E8 + 8);
        Gpu_SetDrawEnable(temp_s0_12, 1);
        temp_s0_12->u = 0x58;
        temp_s0_12->v = markerV;
        temp_s0_12->color.bytes.r = 0x80;
        temp_s0_12->color.bytes.g = 0x80;
        temp_s0_12->color.bytes.b = 0x80;
        *(u16 *)(D_8009E2FE + temp_s4) = hudClut;
        temp_s0_12->width = 8;
        temp_s0_12->height = 4;
        Gpu_InitDrawModeSprtPacket(temp_s4 + D_8009E320, hudPage);
        temp_s1_7 = temp_s4 + (D_8009E320 + 8);
        Gpu_SetDrawEnable(temp_s1_7, 1);
        i = 0;
        temp_s3_4 = bufferIndex * 0x30;
        tileBase = (u32)D_8009E358;
        temp_s1_7->u = 0x60;
        temp_s1_7->v = markerV;
        temp_s1_7->color.bytes.r = 0x80;
        temp_s1_7->color.bytes.g = 0x80;
        temp_s1_7->color.bytes.b = 0x80;
        *(u16 *)(D_8009E336 + temp_s4) = hudClut;
        temp_s1_7->width = 8;
        temp_s1_7->height = 4;

loop_10:
        tileIndex = (u8)i;
        temp_s0_13 = tileIndex * 0x10;
        SetTile(temp_s3_4 + (temp_s0_13 + tileBase));
        i += 1;
        temp_s0_14 = temp_s3_4 + temp_s0_13 + tileBase;
        temp_v0 = (u8)shades.values[tileIndex];
        temp_s0_14->r0 = temp_v0;
        temp_s0_14->g0 = temp_v0;
        temp_s0_14->b0 = temp_v0;

        if ((u32) (i & 0xFF) < 3U) {
            goto loop_10;
        }
        temp_s1_8 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s1_8 + D_8009E460, hudPage);
        temp_s2_4 = (bufferIndex << 1) << 4;
        temp_s4_2 = temp_s2_4 + D_8009E498;
        temp_s0_15 = temp_s1_8 + (D_8009E460 + 8);
        temp_s0_15->u = 0xE8;
        temp_s0_15->v = 0xE0;
        *(u16 *)(D_8009E476 + temp_s1_8) = hudClut;
        temp_s0_15->width = 0x18;
        temp_s0_15->height = 0x18;
        SetLineF2(temp_s4_2);
        temp_s2_5 = temp_s2_4 + (D_8009E498 + 0x10);
        SetLineF2(temp_s2_5);
        temp_s4_2->r = 0xE0;
        temp_s4_2->g = 0xE0;
        temp_s4_2->b = 0xE0;
        temp_s2_5->r = 0x60;
        temp_s2_5->g = 0x60;
        temp_s2_5->b = 0x60;
        SetPolyF3((bufferIndex * 0x14) + D_8009E4D8);
        i = 0;
        temp_s1_9 = bufferIndex * 0x54;
        base2 = (u32)D_8009E3B8;
        body2 = base2 + 8;
loop_12:
        temp_s0_16 = (i & 0xFF) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s1_9 + (temp_s0_16 + base2), hudPage);
        i += 1;
        temp_s0_17 = temp_s0_16 + temp_s1_9;
        *(u16 *)(D_8009E3CE + temp_s0_17) = hudClut;
        temp_s0_18 = temp_s0_17 + body2;
        temp_s0_18->width = 0x18;
        temp_s0_18->height = 8;
        temp_s0_18->color.bytes.r = 0x80;
        temp_s0_18->color.bytes.g = 0x80;
        temp_s0_18->color.bytes.b = 0x80;
        if ((u32) (i & 0xFF) < 3U) {
            goto loop_12;
        }
        i = 0;
        temp_s1_10 = bufferIndex * 0x118;
        base3 = (u32)D_8009E500;
        body3 = base3 + 8;
loop_14:
        temp_s0_19 = (i & 0xFF) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s1_10 + (temp_s0_19 + base3), hudPage);
        i += 1;
        temp_s0_20 = temp_s1_10 + temp_s0_19 + body3;
        temp_s0_20->color.bytes.r = 0x80;
        temp_s0_20->color.bytes.g = 0x80;
        temp_s0_20->color.bytes.b = 0x80;
        if ((u32) (i & 0xFF) < 0xAU) {
            goto loop_14;
        }
        temp_s2_6 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s2_6 + D_8009E768, hudPage);
        i = 0;
        temp_s1_11 = bufferIndex * 0x70;
        base4 = (u32)D_8009E7A0;
        body4 = base4 + 8;
        temp_s0_21 = temp_s2_6 + (D_8009E768 + 8);
        temp_s0_21->u = 0x58;
        temp_s0_21->v = 0xEF;
        *(u16 *)(D_8009E77E + temp_s2_6) = hudClut;
        temp_s0_21->width = 0x24;
        temp_s0_21->height = 5;
        temp_s0_21->color.bytes.r = 0x80;
        temp_s0_21->color.bytes.g = 0x80;
        temp_s0_21->color.bytes.b = 0x80;
loop_16:
        temp_s0_22 = (i & 0xFF) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s1_11 + (temp_s0_22 + base4), hudPage);
        i += 1;
        temp_s0_23 = temp_s0_22 + temp_s1_11;
        *(u16 *)(D_8009E7B6 + temp_s0_23) = hudClut;
        temp_s0_24 = temp_s0_23 + body4;
        temp_s0_24->width = 6;
        temp_s0_24->height = 6;
        temp_s0_24->color.bytes.r = 0x80;
        temp_s0_24->color.bytes.g = 0x80;
        temp_s0_24->color.bytes.b = 0x80;
        if ((u32) (i & 0xFF) < 4U) {
            goto loop_16;
        }
        temp_s2_7 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s2_7 + D_8009E730, hudPage);
        temp_s0_25 = temp_s2_7 + (D_8009E730 + 8);
        temp_s0_25->u = 0x68;
        temp_s0_25->v = 0xF4;
        *(u16 *)(D_8009E746 + temp_s2_7) = GetClut(0x130, 0x1F9);
        temp_s0_25->width = 0x18;
        temp_s0_25->height = 4;
        temp_s0_25->color.bytes.r = 0x80;
        temp_s0_25->color.bytes.g = 0x80;
        temp_s0_25->color.bytes.b = 0x80;
        Gpu_InitDrawModeSprtPacket(temp_s2_7 + D_8009E880, hudPage);
        i = 0;
        temp_s3_5 = bufferIndex * 0x38;
        base5 = (u32)D_8009E8B8;
        body5 = base5 + 8;
        temp_s1_12 = temp_s2_7 + (D_8009E880 + 8);
        temp_s1_12->u = 0x7C;
        temp_s1_12->v = 0xEF;
        *(u16 *)(D_8009E896 + temp_s2_7) = hudClut;
        temp_s1_12->width = 0x24;
        temp_s1_12->height = 5;
        temp_s1_12->color.bytes.r = 0x80;
        temp_s1_12->color.bytes.g = 0x80;
        temp_s1_12->color.bytes.b = 0x80;

loop_18:
        temp_s0_26 = ((u8)i) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s3_5 + (temp_s0_26 + base5), hudPage);
        i += 1;
        temp_s0_27 = temp_s0_26 + temp_s3_5;
        *(u16 *)(D_8009E8CE + temp_s0_27) = hudClut;
        temp_s0_28 = temp_s0_27 + body5;
        temp_s0_28->width = 6;
        temp_s0_28->height = 6;

        if ((u32) (i & 0xFF) < 2U) {
            goto loop_18;
        }
        temp_s0_29 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s0_29 + D_8009E928, hudPage);
        i = 0;
        temp_s4_3 = bufferIndex * 0x16C;
        glyphBase = (u32)D_8009E960;
        body6 = glyphBase + 8;
        *(u16 *)(D_8009E93E + temp_s0_29) = hudClut;
        temp_s0_30 = temp_s0_29 + (D_8009E928 + 8);
        temp_s0_30->color.bytes.r = 0x80;
        temp_s0_30->color.bytes.g = 0x80;
        temp_s0_30->color.bytes.b = 0;

loop_20:
        digitGlyph = Draw_LookupGlyphDescriptor(((u8)i) + 0x6A);
        asm("" : "=r"(digitGlyph) : "0"(digitGlyph));
        pageCode = GetTPage(0, 0, 0x1C0, 0);
        temp_s0_31 = ((u8)i) * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s4_3 + (temp_s0_31 + glyphBase), pageCode);
        temp_s0_32 = temp_s4_3 + temp_s0_31;
        temp_v1 = temp_s0_32 + body6;
        temp_v1->u = (u8) digitGlyph->u;
        temp_v1->v = (u8) digitGlyph->v;
        *(u16 *)(D_8009E976 + temp_s0_32) = digitGlyph->clut;
        temp_v1->width = (s16) digitGlyph->width;
        glyphHeight = digitGlyph->height;
        i += 1;
        temp_v1->color.bytes.r = 0x80;
        temp_v1->color.bytes.g = 0x80;
        temp_v1->color.bytes.b = 0x80;
        temp_v1->height = (s16) glyphHeight;

        if ((u32) (i & 0xFF) < 0xDU) {
            goto loop_20;
        }
        pageCode = GetTPage(0, 0, 0x1C0, 0);
        temp_s0_33 = bufferIndex * 0x1C;
        Gpu_InitDrawModeSprtPacket(temp_s0_33 + D_8009EC38, pageCode);
        temp_s0_34 = temp_s0_33 + (D_8009EC38 + 8);
        bufferIndex += 1;
        temp_s0_34->width = 0x10;
        temp_s0_34->height = 0x10;
        temp_s0_34->color.bytes.r = 0x80;
        temp_s0_34->color.bytes.g = 0x80;
        temp_s0_34->color.bytes.b = 0x80;
    } while (bufferIndex < 2U);
}
