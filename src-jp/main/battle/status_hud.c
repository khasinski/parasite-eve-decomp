/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_main.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
#include "pe1/battle_update.h"
#include "pe1/draw_slot_record.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/global_slot.h"
#include "pe1/menu_state.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_animation_frame.h"
#include "pe1/render_buffers.h"
#include "pe1/render_packets.h"
#include "pe1/render_prim.h"
#include "pe1/script_state.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_BattleStatusPanelSlot;
u8 g_BattleSpritePrimCountdown;
u8 g_MenuActiveMode;
u16 g_MenuActiveWindowXOffset;
int g_BattleAttackAnimFrame;

void Draw_InitDrawModeSprtPacket(void *, int);
void Draw_InitDrawModeTilePacket(void *, int);
void SetSemiTrans(void *, int);
void SetShadeTex(void *, int);
void SetPolyF3(void *);
void SetPolyFT4(RenderTexturedQuad *);
void SetTile(void *);
u16 GetClut(int, int);

/* Initialize the HUD packet pools for both draw buffers. The legacy symbol
 * name is retained; this routine sets up more than the HP bar.
 * Loops that write a sprite after initializing its packet address the sprite
 * through BATTLE_SPRITE_SLOTS, which keeps the sprite base in its own
 * register as the retail code does.
 */
void Battle_DrawHPBar(void) {
    BattleHudShades shades;
    s32 i;
    s32 j;
    u16 pageCode;
    u32 tileIndex;
    s32 highBlue;
    u8 glyphHeight;
    u8 markerV;
    u8 bufferIndex;
    u16 hudClut;
    RenderLinePacket *upperLine;
    RenderLinePacket *lowerLine;
    RenderTexturedQuad *labelQuad;
    s32 hudPage;
    u8 shade;
    RenderSpritePacket *hpLabel;
    RenderSpritePacket *pointerSprite;
    RenderSpritePacket *ammoIcon;
    RenderSpritePacket *statusIcon;
    RenderSpritePacket *ammoLabel;
    RenderSpritePacket *weaponIcon;
    u8 gridRow;
    RenderSpritePacket *cursorSprite;
    RenderColorTilePacket *background;
    RenderGouraudQuad *atbGradient;
    RenderColorTilePacket *atbTrack;
    RenderSpritePacket *atbMarker;
    RenderGouraudQuad *greenGradient;
    RenderSpritePacket *peLabel;
    RenderGouraudQuad *pinkGradient;
    DrawGlyphDescriptor *glyph;
    RenderSpritePacket *atbLabel;

    shades = g_BattleStatusShades[0];
    hudPage = GetTPage(0, 1, 0x100, 0x1E0);
    hudClut = GetClut(0x130, 0x1F8);
    bufferIndex = 0;
    do {
        glyph = Draw_LookupGlyphDescriptor(0x8B);
        labelQuad = &g_BattleEndPanels[bufferIndex];
        SetPolyFT4(labelQuad);
        labelQuad->u0 = glyph->u;
        labelQuad->v0 = glyph->v;
        labelQuad->u1 = glyph->u + glyph->width;
        labelQuad->v1 = glyph->v;
        labelQuad->u2 = glyph->u;
        labelQuad->v2 = glyph->v + glyph->height;
        labelQuad->u3 = glyph->u + glyph->width;
        labelQuad->v3 = glyph->v + glyph->height;
        g_BattleEndPanels[bufferIndex].tpage = GetTPage(0, 0, 0x1C0, 0);
        g_BattleEndPanels[bufferIndex].clut = glyph->clut;
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
        SetSemiTrans(labelQuad, 1);
        {
            u8 buffer;
            buffer = bufferIndex;

            for (i = 0; (u8)i < 10; i++) {
                gridRow = (u8)i;
                for (j = 0; (u8)j < 4; j++) {
                    Draw_InitDrawModeSprtPacket(&g_BattlePanelDigits[buffer][gridRow][(u8)j], hudPage);
                    g_BattlePanelDigits[buffer][gridRow][(u8)j].sprite.clut = hudClut;
                }
            }
        }
        pageCode = GetTPage(0, 0, 0, 0);
        Draw_InitDrawModeTilePacket(&g_BattleStatusBackgrounds[bufferIndex], pageCode);
        background = &g_BattleStatusBackgrounds[bufferIndex].tile;
        background->r0 = 0x30;
        background->g0 = 0x30;
        background->b0 = 0x30;
        SetSemiTrans(background, 1);
        atbTrack = &g_BattleAtbTracks[bufferIndex];
        SetTile(atbTrack);
        atbGradient = &g_BattleAtbGauges[bufferIndex];
        atbTrack->r0 = 0x1D;
        atbTrack->g0 = 0x3E;
        atbTrack->b0 = 0x32;
        atbTrack->w = 0x38;
        atbTrack->h = 3;
        SetPolyG4(atbGradient);
        Draw_InitDrawModeSprtPacket(&g_BattleAtbMarkers[bufferIndex], hudPage);
        atbMarker = &g_BattleAtbMarkers[bufferIndex].sprite;
        atbMarker->u = 0xC8;
        atbMarker->v = 0xE0;
        g_BattleAtbMarkers[bufferIndex].sprite.clut = hudClut;
        atbMarker->width = 4;
        atbMarker->height = 8;
        atbGradient->b0 = highBlue;
        atbGradient->g1 = 0xFF;
        atbGradient->r0 = 0;
        atbGradient->g0 = 0x46;
        atbGradient->r1 = 0x9F;
        atbGradient->b2 = highBlue;
        atbGradient->b1 = 0xF9;
        atbGradient->r2 = 0;
        atbGradient->g2 = 0x46;
        atbGradient->r3 = 0x9F;
        atbGradient->g3 = 0xFF;
        atbGradient->b3 = 0xF9;
        atbMarker->color.bytes.r = 0x9F;
        atbMarker->color.bytes.g = 0xFF;
        atbMarker->color.bytes.b = 0xF9;

        for (i = 0; (u8)i < 4; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleHpDigits[bufferIndex][(u8)i], hudPage);
            g_BattleHpDigits[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleHpDigits, 4)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleHpDigits, 4)[bufferIndex][(u8)i].sprite.height = 0xA;
        }
        for (i = 0; (u8)i < 5; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleMaxHpDigits[bufferIndex][(u8)i], hudPage);
            g_BattleMaxHpDigits[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleMaxHpDigits, 5)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleMaxHpDigits, 5)[bufferIndex][(u8)i].sprite.height = 0xA;
        }
        greenGradient = &g_BattlePeGauges[bufferIndex][0];
        SetPolyG4(greenGradient);
        pinkGradient = &g_BattlePeGauges[bufferIndex][1];
        SetPolyG4(pinkGradient);
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
        BATTLE_SET_GAUGE_GRADIENT(*pinkGradient, 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
        Draw_InitDrawModeSprtPacket(&g_BattleAtbLabels[bufferIndex], hudPage);
        atbLabel = &g_BattleAtbLabels[bufferIndex].sprite;
        SetShadeTex(atbLabel, 1);
        markerV = 0xF4;
        atbLabel->u = 0x50;
        atbLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*atbLabel, 0x80, 0x80, 0x80);
        g_BattleAtbLabels[bufferIndex].sprite.clut = hudClut;
        atbLabel->width = 8;
        atbLabel->height = 4;
        Draw_InitDrawModeSprtPacket(&g_BattleHpLabels[bufferIndex], hudPage);
        hpLabel = &g_BattleHpLabels[bufferIndex].sprite;
        SetShadeTex(hpLabel, 1);
        hpLabel->u = 0x58;
        hpLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*hpLabel, 0x80, 0x80, 0x80);
        g_BattleHpLabels[bufferIndex].sprite.clut = hudClut;
        hpLabel->width = 8;
        hpLabel->height = 4;
        Draw_InitDrawModeSprtPacket(&g_BattlePeLabels[bufferIndex], hudPage);
        peLabel = &g_BattlePeLabels[bufferIndex].sprite;
        SetShadeTex(peLabel, 1);
        peLabel->u = 0x60;
        peLabel->v = markerV;
        BATTLE_SET_SPRITE_COLOR(*peLabel, 0x80, 0x80, 0x80);
        g_BattlePeLabels[bufferIndex].sprite.clut = hudClut;
        peLabel->width = 8;
        peLabel->height = 4;

        for (i = 0; (u8)i < 3; i++) {
            tileIndex = (u8)i;
            SetTile(&BATTLE_STATUS_TILES[bufferIndex][tileIndex]);
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].r0 = shade = shades.values[tileIndex];
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].g0 = shade;
            BATTLE_STATUS_TILES[bufferIndex][tileIndex].b0 = shade;
        }
        Draw_InitDrawModeSprtPacket(&g_BattleStatusPointerSprites[bufferIndex], hudPage);
        upperLine = &g_BattleStatusPointerLines[bufferIndex][0];
        pointerSprite = &g_BattleStatusPointerSprites[bufferIndex].sprite;
        pointerSprite->u = 0xE8;
        pointerSprite->v = 0xE0;
        g_BattleStatusPointerSprites[bufferIndex].sprite.clut = hudClut;
        pointerSprite->width = 0x18;
        pointerSprite->height = 0x18;
        SetLineF2(upperLine);
        lowerLine = &g_BattleStatusPointerLines[bufferIndex][1];
        SetLineF2(lowerLine);
        upperLine->r = 0xE0;
        upperLine->g = 0xE0;
        upperLine->b = 0xE0;
        lowerLine->r = 0x60;
        lowerLine->g = 0x60;
        lowerLine->b = 0x60;
        SetPolyF3(&g_BattleStatusPointerTriangles[bufferIndex]);
        for (i = 0; (u8)i < 3; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleTargetHighlightSprites[bufferIndex][(u8)i], hudPage);
            g_BattleTargetHighlightSprites[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleTargetHighlightSprites, 3)[bufferIndex][(u8)i].sprite.width = 0x18;
            BATTLE_SPRITE_SLOTS(g_BattleTargetHighlightSprites, 3)[bufferIndex][(u8)i].sprite.height = 8;
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(g_BattleTargetHighlightSprites, 3)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        for (i = 0; (u8)i < 10; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleRemainingActionSprites[bufferIndex][(u8)i], hudPage);
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(g_BattleRemainingActionSprites, 10)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        Draw_InitDrawModeSprtPacket(&g_BattleAmmoLabelSprites[bufferIndex], hudPage);
        ammoLabel = &g_BattleAmmoLabelSprites[bufferIndex].sprite;
        ammoLabel->u = 0x58;
        ammoLabel->v = 0xEF;
        g_BattleAmmoLabelSprites[bufferIndex].sprite.clut = hudClut;
        ammoLabel->width = 0x24;
        ammoLabel->height = 5;
        BATTLE_SET_SPRITE_COLOR(*ammoLabel, 0x80, 0x80, 0x80);
        for (i = 0; (u8)i < 4; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleAmmoDigitSprites[bufferIndex][(u8)i], hudPage);
            g_BattleAmmoDigitSprites[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleAmmoDigitSprites, 4)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleAmmoDigitSprites, 4)[bufferIndex][(u8)i].sprite.height = 6;
            BATTLE_SET_SPRITE_COLOR(BATTLE_SPRITE_SLOTS(g_BattleAmmoDigitSprites, 4)[bufferIndex][(u8)i].sprite,
                                    0x80, 0x80, 0x80);
        }
        Draw_InitDrawModeSprtPacket(&g_BattleStatusValueLabelSprites[bufferIndex], hudPage);
        statusIcon = &g_BattleStatusValueLabelSprites[bufferIndex].sprite;
        statusIcon->u = 0x68;
        statusIcon->v = 0xF4;
        g_BattleStatusValueLabelSprites[bufferIndex].sprite.clut = GetClut(0x130, 0x1F9);
        statusIcon->width = 0x18;
        statusIcon->height = 4;
        BATTLE_SET_SPRITE_COLOR(*statusIcon, 0x80, 0x80, 0x80);
        Draw_InitDrawModeSprtPacket(&g_BattleStatusValueMarkers[bufferIndex], hudPage);
        ammoIcon = &g_BattleStatusValueMarkers[bufferIndex].sprite;
        ammoIcon->u = 0x7C;
        ammoIcon->v = 0xEF;
        g_BattleStatusValueMarkers[bufferIndex].sprite.clut = hudClut;
        ammoIcon->width = 0x24;
        ammoIcon->height = 5;
        BATTLE_SET_SPRITE_COLOR(*ammoIcon, 0x80, 0x80, 0x80);
        for (i = 0; (u8)i < 2; i++) {
            Draw_InitDrawModeSprtPacket(&g_BattleStatusValueDigits[bufferIndex][(u8)i], hudPage);
            g_BattleStatusValueDigits[bufferIndex][(u8)i].sprite.clut = hudClut;
            BATTLE_SPRITE_SLOTS(g_BattleStatusValueDigits, 2)[bufferIndex][(u8)i].sprite.width = 6;
            BATTLE_SPRITE_SLOTS(g_BattleStatusValueDigits, 2)[bufferIndex][(u8)i].sprite.height = 6;
        }
        Draw_InitDrawModeSprtPacket(&g_BattleStatusSymbolSprites[bufferIndex], hudPage);
        g_BattleStatusSymbolSprites[bufferIndex].sprite.clut = hudClut;
        weaponIcon = &g_BattleStatusSymbolSprites[bufferIndex].sprite;
        BATTLE_SET_SPRITE_COLOR(*weaponIcon, 0x80, 0x80, 0);

        for (i = 0; (u8)i < 13; i++) {
            glyph = Draw_LookupGlyphDescriptor(((u8)i) + 0x6A);
            pageCode = GetTPage(0, 0, 0x1C0, 0);
            Draw_InitDrawModeSprtPacket(&g_StatusIconPackets[bufferIndex][(u8)i], pageCode);
#define GLYPH_SPRITE BATTLE_SPRITE_SLOTS(g_StatusIconPackets, 13)[bufferIndex][(u8)i].sprite
            GLYPH_SPRITE.u = glyph->u;
            GLYPH_SPRITE.v = glyph->v;
            g_StatusIconPackets[bufferIndex][(u8)i].sprite.clut = glyph->clut;
            GLYPH_SPRITE.width = glyph->width;
            glyphHeight = glyph->height;
            BATTLE_SET_SPRITE_COLOR(GLYPH_SPRITE, 0x80, 0x80, 0x80);
            GLYPH_SPRITE.height = glyphHeight;
#undef GLYPH_SPRITE
        }
        pageCode = GetTPage(0, 0, 0x1C0, 0);
        Draw_InitDrawModeSprtPacket(&g_StatusMarkerPackets[bufferIndex], pageCode);
        cursorSprite = &g_StatusMarkerPackets[bufferIndex].sprite;
        bufferIndex += 1;
        cursorSprite->width = 0x10;
        cursorSprite->height = 0x10;
        BATTLE_SET_SPRITE_COLOR(*cursorSprite, 0x80, 0x80, 0x80);
    } while (bufferIndex < 2U);
}

void Battle_BuildStatusPrimHeader(RenderObjectEntity *, s8);
void Battle_DrawSpellName(void);
void Battle_DrawStatusSymbol(s8);
void Battle_DrawStatusOverlay(RenderObjectEntity *object, int mode, int outOfRange, int entityId) {
    register int modeArg asm("$5") = mode;
    if ((s8)modeArg != 8) {
        register int modeMinusOne asm("$2") = modeArg - 1;
        if ((u8)modeMinusOne < 2) {
            u32 slot = g_ActiveDrawSlot;
            g_BattleStatusBarTiles[slot * 3].x0 = 0x78;
            g_BattleStatusBarTiles[slot * 3].y0 = 0x68;
        } else {
            Battle_BuildStatusPrimHeader(object, (s8)outOfRange);
        }
        switch ((s8)mode) {
        case 0:
            Battle_LayoutStatusPrimRow(0x28);
            Battle_DrawSpellName();
            Battle_DrawATBGauge();
            Battle_DrawStatusValue((s16)entityId, 0x1E);
            break;
        case 1:
        case 2:
            Battle_LayoutStatusPrimRow(0x20);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawATBGauge();
            break;
        case 3: {
            u32 slot;
            u32 packetOffset;
            u8 *packetBase;
            RenderSpritePacket *packet;
            u8 *packetTagBase;
            Battle_LayoutStatusPrimRow(0x1C);
            packetBase = (u8 *)&g_BattleStatusValueLabelSprites[0].sprite;
            slot = g_ActiveDrawSlot;
            packetOffset = slot * 28;
            packet = (RenderSpritePacket *)(packetBase + packetOffset);
            packet->x = g_BattleStatusBarTiles[slot * 3].x0 + 0x1C;
            packet->y = g_BattleStatusBarTiles[slot * 3].y0 + 8;
            /* Keep this assignment at the call to preserve the original schedule. */
            AddPrim((unsigned int *)D_800B08F8.ordering[slot] + 4,
                    (unsigned int *)((packetTagBase = packetBase - 8) + packetOffset));
            Battle_DrawStatusValue((s16)entityId, 0x12);
            break;
        }
        case 4:
            Battle_LayoutStatusPrimRow(0x28);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawATBGauge();
            Battle_DrawStatusValue((s16)entityId, 0x1E);
            break;
        case 5:
        case 6:
        case 7:
            Battle_LayoutStatusPrimRow(0x20);
            Battle_DrawStatusSymbol((s8)mode);
            Battle_DrawStatusValue((s16)entityId, 0x16);
            break;
        }
    } else {
        u32 slot = g_ActiveDrawSlot;
        g_BattleStatusBarTiles[slot * 3].x0 = 0x78;
        g_BattleStatusBarTiles[slot * 3].y0 = 0x6C;
        Battle_LayoutStatusPrimRow(0x18);
        Battle_DrawStatusSymbol(8);
    }
    if (((BattleTurnBits *)&g_ActiveActor->action->turnWord)->slot >= 2)
        Battle_DrawTargetHighlight();
}

/* Builds the rotating target pointer and the two connector lines that tie
 * the floating status panel to the selected combatant. */

/* Packets of the active draw slot. Every access re-reads the slot, which
 * retail does after each byte store. */
#define STATUS_POINTER_SPRITE (&g_BattleStatusPointerSprites[g_ActiveDrawSlot].sprite)
#define STATUS_POINTER_TRI (&g_BattleStatusPointerTriangles[g_ActiveDrawSlot])

void Battle_BuildStatusPrimHeader(RenderObjectEntity *object, s8 outOfRange)
{
    GteMatrix rotation;
    GteMatrix translation;
    GteShortVector pointer[3] = { { -3, -16, 0 }, { 3, -16, 0 }, { 0, 0, 0 } };
    GteVector projected[3];
    GteShortVector angles = { 0, 0, (g_FrameRngCounter << 6) & 0xFFF };
    GteVector offset = { object->projected_target_x, object->projected_target_y, 0 };
    s32 flag;
    s16 x;
    s16 y;

    STATUS_POINTER_SPRITE->x = object->projected_target_x - 12;
    STATUS_POINTER_SPRITE->y = object->projected_target_y - 12;
    if (outOfRange == 0) {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x96;
        STATUS_POINTER_SPRITE->color.bytes.g = 0x14;
        STATUS_POINTER_SPRITE->color.bytes.b = 0x14;
        STATUS_POINTER_TRI->r = 0x96;
        STATUS_POINTER_TRI->g = 0x14;
        STATUS_POINTER_TRI->b = 0x14;
    } else {
        STATUS_POINTER_SPRITE->color.bytes.r = 0x32;
        STATUS_POINTER_SPRITE->color.bytes.g = 0xA;
        STATUS_POINTER_SPRITE->color.bytes.b = 0xA;
        STATUS_POINTER_TRI->r = 0x32;
        STATUS_POINTER_TRI->g = 0xA;
        STATUS_POINTER_TRI->b = 0xA;
    }

    RotMatrix(&angles, &rotation);
    SetRotMatrix(&rotation);
    TransMatrix(&translation, &offset);
    SetTransMatrix(&translation);
    RotTrans(&pointer[0], &projected[0], &flag);
    RotTrans(&pointer[1], &projected[1], &flag);
    RotTrans(&pointer[2], &projected[2], &flag);
    STATUS_POINTER_TRI->x0 = projected[0].x;
    STATUS_POINTER_TRI->y0 = projected[0].y;
    STATUS_POINTER_TRI->x1 = projected[1].x;
    STATUS_POINTER_TRI->y1 = projected[1].y;
    STATUS_POINTER_TRI->x2 = projected[2].x;
    STATUS_POINTER_TRI->y2 = projected[2].y;
    AddPrim((unsigned int *)D_800B08F8.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)STATUS_POINTER_TRI);

    /* Connector lines run to the panel on whichever side has room. */
    x = object->projected_target_x;
    if (x + 134 >= 301) {
        RenderLinePacket *lines;
        s16 panelX;

        x -= 8;
        lines = g_BattleStatusPointerLines[g_ActiveDrawSlot];
        lines[1].x0 = x;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].x0 = x;
        panelX = object->projected_target_x;
        lines[1].x1 = panelX - 35;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].x1 = panelX - 35;
        g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].x0 = panelX - 115;
    } else {
        RenderLinePacket *lines;
        s16 lineX;
        s16 panelX;

        lineX = x + 8;
        lines = g_BattleStatusPointerLines[g_ActiveDrawSlot];
        lines[1].x0 = lineX;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].x0 = lineX;
        panelX = object->projected_target_x + 35;
        g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].x0 = panelX;
        lines[1].x1 = panelX;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].x1 = panelX;
    }
    y = object->projected_target_y;
    if (y - 82 < 0) {
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].y0 = y + 8;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][1].y0 = object->projected_target_y + 9;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].y1 = g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y + 35;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][1].y1 = object->projected_target_y + 36;
    } else {
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].y0 = y - 8;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][1].y0 = object->projected_target_y - 7;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][0].y1 = g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].y0 =
            object->projected_target_y - 35;
        g_BattleStatusPointerLines[g_ActiveDrawSlot][1].y1 = object->projected_target_y - 34;
    }
    AddPrim((unsigned int *)D_800B08F8.ordering[g_ActiveDrawSlot] + 7,
            (unsigned int *)&g_BattleStatusPointerLines[g_ActiveDrawSlot][1]);
    AddPrim((unsigned int *)D_800B08F8.ordering[g_ActiveDrawSlot] + 6,
            (unsigned int *)&g_BattleStatusPointerLines[g_ActiveDrawSlot][0]);
    AddPrim((unsigned int *)D_800B08F8.ordering[g_ActiveDrawSlot] + 5,
            (unsigned int *)&g_BattleStatusPointerSprites[g_ActiveDrawSlot]);
}

void Battle_LayoutStatusPrimRow(int bottomY)
{
    BattleStatusLinePrim *line;
    BattleStatusPrimAddress bufferBase;
    unsigned int **orderingBase;
    int savedBottomY;
    int slot;
    u8 row;

    savedBottomY = bottomY;
    row = 0;
    bufferBase.line = g_BattleStatusBarTiles;
    orderingBase = D_800B08F8.ordering;
    do {
        slot = g_ActiveDrawSlot;
        line = (BattleStatusLinePrim *)
            (((slot * 3) << 4) + (row << 4) + bufferBase.address);
        line->x0 = g_BattleStatusBarTiles[slot * 3].x0 + row;
        line->y0 = g_BattleStatusBarTiles[slot * 3].y0 + row;
        line->x1 = 0x50 - (row * 2);
        line->y1 = (s8)savedBottomY - (row * 2);
        AddPrim(orderingBase[slot] + (7 - row),
                (unsigned int *)(((slot * 3) << 4) +
                    ((row << 4) + bufferBase.address)));
        row++;
    } while (row < 3);
}

static inline RenderSpritePacket *HighlightSpriteAt(RenderSpritePacket *base, int slot, unsigned byte_offset)
{
    BattleStatusPrimAddress at;

    at.sprite = base;
    at.address = slot * 3 * sizeof(BattleGaugePrim) + byte_offset + at.address;
    return at.sprite;
}

void Battle_DrawTargetHighlight(void)
{
    unsigned char index = 0;
    RenderSpritePacket *base;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packets;
    int slot;
    unsigned slot_offset;
    register unsigned first_offset asm("$4");
    register unsigned index_offset asm("$5");
    register unsigned char texture_v asm("$18");
    if (((g_ActiveActor->action->turnWord >> 4) & 3) == 0) {
        return;
    }
    texture_v = 224;
    base = &g_BattleTargetHighlightSprites[0][0].sprite;
    packets = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));
    do {
        if (index == g_BattleActiveTurnSlot - 1) {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = index * 24 + 104;
        } else {
            first_offset = index * sizeof(BattleGaugePrim);
            HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->u = 176;
        }
        HighlightSpriteAt(base, g_ActiveDrawSlot, first_offset)->v = texture_v;
        /* Preserve the retail store before recomputing the next packet offset. */
        __asm__ volatile("" : : : "memory");
        index_offset = index * sizeof(BattleGaugePrim);
        slot = g_ActiveDrawSlot;
        slot_offset = slot * 3 * sizeof(BattleGaugePrim);
        {
            BattleStatusPrimAddress at;

            at.sprite = base;
            at.address = slot_offset + index_offset + at.address;
            sprite = at.sprite;
        }
        sprite->x = g_BattleStatusBarTiles[slot * 3].x0 + index * 24;
        sprite->y = g_BattleStatusBarTiles[slot * 3].y0 - 8;
        {
            BattleStatusPrimAddress at;

            at.gauge = packets;
            at.address = slot_offset + (index_offset + at.address);
            AddPrim((unsigned *)D_800B08F8.ordering[slot] + 7, at.words);
        }
        ++index;
    } while (index < (int)((g_ActiveActor->action->turnWord >> 4) & 3));
}

#define NULL ((void *)0)

void Battle_DrawSpellName(void) {
    s32 c;
    register s32 packetOffset asm("$3");
    register s32 workValue asm("$4");
    s32 c1;
    s16 ys;
    s16 xs;
    s16 i;
    u8 *e508;
    s32 cidx;
    s16 clut;
    u8 *p;
    BattleAction *d68;

    c = g_ActiveDrawSlotRecord.slot;
    {
        s32 tv;
        tv = *(u16 *)(g_BattleStatusRowPosY + c * 0x30);
        ys = tv + 4;
    }
    if ((g_ActiveActor->action->attackWord & 0x300000) == 0x200000) {
        u8 *q508;
        workValue = 0x130;
        __asm__ __volatile__("" :: "r"(workValue));
        q508 = (u8 *)&g_BattleRemainingActionSprites[0][0].sprite;
        {
            RenderSpritePacket *pa;
            u8 e0a;
            pa = (RenderSpritePacket *)(q508 + c * 0x118);
            e0a = 0xE0;
                        pa->u = e0a;
            {
                s32 cB;
                cB = g_ActiveDrawSlotRecord.slot;
                pa->width = 8;
                pa->height = 0x10;
                (q508 + cB * 0x118)[0xD] = e0a;
            }
        }
        {
            s32 cC;
            BattleStatusPrimAddress t3;
            BattleStatusPrimAddress base;
            s32 rFD;
            s32 tE;
            cC = g_ActiveDrawSlotRecord.slot;
            rFD = 0x1FD;
            t3.address = cC * 0x118;
            tE = *(u16 *)(g_BattleStatusRowPosX + cC * 0x30);
            base.bytes = q508;
            t3.address = t3.address + base.address;
            t3.sprite->y = ys;
            t3.sprite->x = tE + 0x3C;
            clut = GetClut(workValue, rFD);
        }
        {
            s32 cD;
            s32 eo;
            cD = g_ActiveDrawSlotRecord.slot;
            q508 -= 8;
            eo = cD * 0x118;
            cD = cD * 4;
            *(s16 *)(g_BattleRemainingActionClut + eo) = clut;
            __asm__ __volatile__("");
            {
                BattleStatusPrimAddress entry;

                entry.byteOrdering = g_OtBufferTable;
                entry.address = cD + entry.address;
                AddPrim(*entry.byteOrdering + 0x10, eo + q508);
            }
        }
        return;
    }
    i = 0;
    {
        s32 tx;
        tx = *(u16 *)(g_BattleStatusRowPosX + c * 0x30);
        xs = tx + 0x40;
    }
    if (g_BattleRemainingActions != 0) {
        u8 e0 = 0xE0;
        u8 *e5m8;
        e508 = (u8 *)&g_BattleRemainingActionSprites[0][0].sprite;
        e5m8 = e508 - 8;
        do {
            s32 s0;
            RenderSpritePacket *pl;
            {
                {
                    s32 cE;
                    cE = g_ActiveDrawSlotRecord.slot;
                    packetOffset = cE * 0x118;
                    s0 = i * 0x1C;
                }
                packetOffset += s0;
                {
                    BattleStatusPrimAddress at;

                    at.bytes = e508;
                    at.address = packetOffset + at.address;
                    pl = at.sprite;
                }
            }
            {
                Combatant *dval;
                dval = g_ActiveActor;
                pl->width = 4;
                pl->height = 0x10;
                d68 = dval->action;
            }
            if ((d68->attackWord & 0x300000) == 0x300000) {
                workValue = 0x130;
                                pl->u = 0xD8;
                c1 = g_ActiveDrawSlotRecord.slot;
                {
                    BattleStatusPrimAddress at;

                    at.bytes = e508;
                    at.address = c1 * 0x118 + s0 + at.address;
                    at.sprite->v = e0;
                }
                clut = GetClut(workValue, 0x1FB);
            } else if ((d68->turnWord & 0xC0) == 0x80) {
                workValue = 0x130;
                                pl->u = 0xDC;
                c1 = g_ActiveDrawSlotRecord.slot;
                {
                    BattleStatusPrimAddress at;

                    at.bytes = e508;
                    at.address = c1 * 0x118 + s0 + at.address;
                    at.sprite->v = e0;
                }
                clut = GetClut(workValue, 0x1FC);
            } else {
                u8 dv = 0xD4;
                workValue = 0x130;
                                pl->u = dv;
                c1 = g_ActiveDrawSlotRecord.slot;
                {
                    BattleStatusPrimAddress at;

                    at.bytes = e508;
                    at.address = c1 * 0x118 + s0 + at.address;
                    at.sprite->v = e0;
                }
                clut = GetClut(workValue, 0x1FA);
            }
            {
                workValue = g_ActiveDrawSlotRecord.slot;
                packetOffset = workValue * 0x118;
                packetOffset = s0 + packetOffset;
                                *(s16 *)(g_BattleRemainingActionClut + packetOffset) = clut;
                __asm__ __volatile__("");
            }
            {
                s32 j7;
                s32 t2;
                register s32 cJ asm("$6");
                register s32 a2v asm("$5");
                {
                    workValue = i;
                    __asm__("" : "=r"(workValue) : "0"(workValue));
                    j7 = workValue * 7;
                }
                j7 <<= 2;
                cJ = g_ActiveDrawSlotRecord.slot;
                {
                    BattleStatusPrimAddress base;

                    base.bytes = e5m8;
                    a2v = j7 + base.address;
                }
                t2 = cJ * 0x118;
                a2v = t2 + a2v;
                t2 = t2 + j7;
                {
                    BattleStatusPrimAddress at;

                    at.bytes = e508;
                    at.address = t2 + at.address;
                    at.sprite->x = xs;
                    at.sprite->y = ys;
                }
                xs -= 6;
                AddPrim(g_OtBufferTable[cJ] + 0x10, (void *)a2v);
            }
            {
                s16 inext = i + 1;
                i = inext;
                if (!(inext < g_BattleRemainingActions)) break;
            }
        } while (1);
    }
}

typedef struct StatusSymbolSlot {
    StatusSymbolTemplate data;
    u8 padding[6];
} StatusSymbolSlot;

void Battle_DrawStatusSymbol(s8 which) {
    StatusSymbolSlot first;
    StatusSymbolSlot second;
    StatusSymbolSlot third;
    u8 spare[8]; /* Preserve the original stack frame around the copied templates. */
    u32 slot;
    u8 *packet;
    u32 packetOffset;
    u32 index;
    u8 *a, *b, *c;
    u8 *packetBase;
    u8 *thirdBase;
    first.data = g_BattleStatusSymbolUv;
    second.data = g_BattleStatusSymbolSize;
    third.data = g_BattleStatusSymbolOffset;
    index = which * 2;
    a = first.data.bytes + index;
    b = second.data.bytes + index;
    packetBase = (u8 *)&g_BattleStatusSymbolSprites[0].sprite;
    thirdBase = third.data.bytes;
    g_BattleStatusSymbolSpriteU[g_ActiveDrawSlot * 28] = a[0];
    c = thirdBase + index;
    g_BattleStatusSymbolSpriteV[g_ActiveDrawSlot * 28] = a[1];
    slot = g_ActiveDrawSlot;
    packetOffset = slot * 28;
    packet = packetBase + packetOffset;
    ((RenderSpritePacket *)packet)->width = b[0];
    ((RenderSpritePacket *)packet)->height = b[1];
    ((RenderSpritePacket *)packet)->x = *(u16 *)(g_BattleStatusRowPosX + slot * 48) + c[0];
    ((RenderSpritePacket *)packet)->y = ((u16 *)(g_BattleStatusRowPosX + slot * 48))[1] + c[1];
    packetBase -= 8;
    AddPrim(D_800B08F8.ordering[slot] + 4, (unsigned int *)(packetBase + packetOffset));
}

void Battle_DrawATBGauge(void)
{
    short ammo;
    signed char i;
    short y;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packet;
    RenderSpritePacket *base;
    unsigned category;
    /* Keep the queue index initialized across the ammo-base callback. */
    i = 0;
    y = g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].y0 + 22;
    category = (g_ActiveActor->action->attackWord >> 20) & 3;
    ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
    base = &g_BattleAmmoLabelSprites[0].sprite;
    packet = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));

    packet += g_ActiveDrawSlot;
    sprite = &g_BattleAmmoLabelSprites[g_ActiveDrawSlot].sprite;
    ammo += *(unsigned short *)&g_ActiveActor->action->attackWord & 0x3ff;
    sprite->x = g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].x0 + 8;
    sprite->y = y;
    AddPrim((unsigned *)D_800B08F8.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    while (i < Pad_GetMenuPressedBitOrDisabled()) {
        int kind = g_BattleActionQueue[i].command;
        if (kind == 1) {
            --ammo;
        } else if (kind == 2) {
            unsigned mode = g_ActiveActor->action->turnWord & 0xc0;
            if (mode == 0xc0) {
                ammo -= *(unsigned char *)&g_ActiveActor->action->turnWord & 15;
            } else if (mode == 0x40) {
                --ammo;
            }
        } else if (kind == 0x189) {
            --ammo;
        }
        ++i;
    }
    if (ammo < 0) {
        ammo = 0;
    }
    Battle_DrawDecimalNumber(g_BattleAmmoDigitSprites[g_ActiveDrawSlot], g_BattleStatusBarTiles[g_ActiveDrawSlot * 3].x0 + 64, y - 1, ammo, 0);
}

void Battle_DrawStatusValue(int value, int yOffset)
{
    int drawSlot;
    int anchorOffset;
    int markerOffset;
    u8 *markerBase;
    int activeSlot;
    BattleStatusMarkerBody *marker;
    int savedValue;
    register u64 x;
    u16 y;
    u32 drawSlotOffset;

    activeSlot = g_ActiveDrawSlot;
    savedValue = value;
    drawSlot = activeSlot;
    markerBase = (u8 *)g_BattleStatusValueMarkerSlots;
    anchorOffset = (drawSlot * 3) << 4;
    markerOffset = drawSlot * sizeof(BattleStatusMarkerBody);
    do {
        do {
            marker = (BattleStatusMarkerBody *)(markerBase + markerOffset);
        } while (0);
        drawSlotOffset = drawSlot << 2;
        markerBase -= 8;
        {
            BattleStatusPrimAddress anchor;

            anchor.line = g_BattleStatusBarTiles;
            anchor.address = anchorOffset + anchor.address;
            y = anchor.line->y0;
        }
        x = (marker->x =
            *(u16 *)(g_BattleStatusRowPosX + anchorOffset) + 8);
        y += yOffset;
    } while (0);
    marker->y = y;

    {
        BattleStatusPrimAddress entry;

        entry.ordering = D_800B08F8.ordering;
        entry.address = drawSlotOffset + entry.address;
        AddPrim(*entry.ordering + 4, (unsigned int *)(markerBase + markerOffset));
    }
    drawSlot = g_ActiveDrawSlot;
    Battle_DrawDecimalNumber(
        g_BattleStatusValueDigits[drawSlot],
        g_BattleStatusBarTiles[drawSlot * 3].x0 + 0x40,
        (s16)--y, (s16)savedValue, 1);
}

int Battle_DrawDecimalNumber(void *buffer, short x, short y, short value, short mode)
{
    unsigned char digits[6];
    signed char index = 0;
    signed char last;
    short remaining;
    short quotient;
    BattleGaugePrim *packet;
    /* Collect least-significant digits, then draw from the highest place. */
    remaining = value;
    do {
        quotient = remaining / 10;
        digits[index] = remaining - quotient * 10;
        remaining = quotient;
        if (!remaining) {
            break;
        }
        ++index;
    } while (1);
    last = index;
    x -= last * 6;
    for (; index >= 0; --index) {
        unsigned digit;
        /* Retail computes the packet address as a 32-bit integer sum. */
        {
            BattleStatusPrimAddress at;

            at.words = buffer;
            at.address = index * sizeof(*packet) + at.address;
            packet = at.gauge;
        }
        digit = digits[index];
        packet->sprite.v = 0xf2;
        packet->sprite.x = x;
        packet->sprite.y = y;
        packet->sprite.u = digit * 8;
        if (mode == 1) {
            packet->sprite.color.bytes.r = g_BattleStatusPointerSprites[g_ActiveDrawSlot].sprite.color.bytes.r + 40;
            packet->sprite.color.bytes.g = g_BattleStatusPointerSprites[g_ActiveDrawSlot].sprite.color.bytes.g;
            packet->sprite.color.bytes.b = g_BattleStatusPointerSprites[g_ActiveDrawSlot].sprite.color.bytes.b;
        }
        x += 6;
        AddPrim((unsigned *)D_800B08F8.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    }
    return last;
}

/* Digit sprite i of the current draw buffer and panel slot. Every use
 * indexes again: the byte stores make GCC reload both indices. */
#define PANEL_DIGIT(i) (&g_BattlePanelDigits[g_ActiveDrawSlot][g_BattleStatusPanelSlot][i].sprite)

/* Draw a floating damage/recovery number, or the enemy MISS label.
 * Each frame has ten panel slots (0x578 / 0x8C), each with five 28-byte
 * texture-page/sprite packets. Retail sizes the digit buffer at 56 bytes
 * although a 16-bit value needs at most five digits.
 * Matching debt: the MISS branch keeps seven pins and two barriers; the
 * crutch-free form wastes stack slots (see CODE_ORGANIZATION.md).
 */
void Battle_DrawStatusPanel(s32 mode, BattleStatusPanel *input) {
    BattleStatusPanel *panel = input;
    u8 digits[56];
    s32 screenY;
    s16 panelValue;
    register s32 number asm("$6");
    s32 screenX;
    s32 missFrameOffset;
    s32 panelOffset;
    s32 timer;
    register RenderSpritePacket *missSprite asm("$2");
    s16 value;
    s16 quotient;
    s8 n;
    s32 rise;

    panelValue = panel->value;
    number = panelValue;
    if ((panelValue < 0) && (number = 0, ((mode & 0xFF) == 1))) {
        register s32 frame asm("$2") = g_ActiveDrawSlot;
        register s32 slotIndex asm("$4") = g_BattleStatusPanelSlot;
        register s32 x asm("$6") = panel->x;
        register s32 y asm("$7");
        timer = panel->timer;
        y = panel->y;
        *(g_BattleMissSpriteR + ((slotIndex * 0x8C) + (frame * 0x578))) = timer * 4;
        asm("" : : : "memory");
        screenX = x - 16;
        asm("" : : "r"(screenX));
        timer -= 30;
        screenY = y + timer;
        *(g_BattleMissSpriteG + ((g_BattleStatusPanelSlot * 0x8C) + (g_ActiveDrawSlot * 0x578))) = 0;
        *(g_BattleMissSpriteB + ((g_BattleStatusPanelSlot * 0x8C) + (g_ActiveDrawSlot * 0x578))) = 0;
        *(g_BattleMissSpriteU + ((g_BattleStatusPanelSlot * 0x8C) + (g_ActiveDrawSlot * 0x578))) = 0x50;
        *(g_BattleMissSpriteV + ((g_BattleStatusPanelSlot * 0x8C) + (g_ActiveDrawSlot * 0x578))) = 0xE0;
        {
            /* screenY is complete; reuse the Y scratch for the draw slot. */
            y = g_ActiveDrawSlot;
            missFrameOffset = y * 0x578;
            panelOffset = g_BattleStatusPanelSlot * 0x8C;
            missSprite = (RenderSpritePacket *)(missFrameOffset + panelOffset + (int)g_BattleMissSprite);
            missSprite->width = 0x18;
            missSprite->x = screenX;
            missSprite->y = screenY;
            missSprite->height = 8;
            AddPrim(((u32 *)D_800B08F8.ordering[y]) + 7, (void *)((((int)g_BattleMissSprite - 8) + missFrameOffset) + panelOffset));
        }
    } else {
        value = number;
        n = 0;
        for (;;) {
            quotient = value / 10;
            digits[n] = value - quotient * 10;
            value = quotient;
            if (value == 0)
                break;
            n++;
        }
        screenX = panel->x - (n + 1) * 4;
        rise = panel->timer - 30;
        screenY = panel->y + rise;
        for (; n >= 0; n--) {
            switch (panel->style) {
            case 0:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = panel->timer * 4;
                break;
            case 1:
                PANEL_DIGIT(n)->color.bytes.r = 0;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = 0;
                break;
            case 2:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = 0;
                break;
            case 3:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = 0;
                PANEL_DIGIT(n)->color.bytes.b = panel->timer * 4;
                break;
            }
            screenX += 8;
            PANEL_DIGIT(n)->u = digits[n] * 8;
            PANEL_DIGIT(n)->v = 0xE0;
            PANEL_DIGIT(n)->x = screenX;
            PANEL_DIGIT(n)->y = screenY;
            PANEL_DIGIT(n)->width = 8;
            PANEL_DIGIT(n)->height = 8;
            AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 7, (u32 *)&g_BattlePanelDigits[g_ActiveDrawSlot][g_BattleStatusPanelSlot][n]);
        }
    }
    g_BattleStatusPanelSlot += 1;
}

void Draw_SetupSprites(BattleEntity *entity, int unused, int icon) {
    int i;

    g_BattleSpritePrimCountdown = 0x1E;
    i = 0;
    icon = (u8)icon;
    do {
        RenderSpritePacket *marker;

        marker = &g_StatusMarkerPackets[(u8)i].sprite;
        marker->u = g_StatusIconPackets[(u8)i][icon].sprite.u;
        marker->v = g_StatusIconPackets[(u8)i][icon].sprite.v;
        marker->x = entity->renderObject.projected_x - 8;
        marker->y = entity->renderObject.projected_y - 0x10;
        g_StatusMarkerPackets[(u8)i].sprite.clut =
            g_StatusIconPackets[(u8)i][icon].sprite.clut;
        i++;
    } while ((u8)i < 2);
}

void Draw_QueuePrimitive(void) {
    int slot;

    slot = g_ActiveDrawSlot;
    g_StatusMarkerPackets[slot].sprite.y -= 2;
    AddPrim((u32 *)g_OtBufferTable[slot] + 7, &g_StatusMarkerPackets[slot]);
    g_BattleSpritePrimCountdown--;
}

/* Each emit queues one status icon at the cursor and advances the cursor.
 * Retail adds the draw-slot and icon offsets to the sprite base and steps
 * back 8 bytes for the packet; see CODE_ORGANIZATION.md. */
#define INDEX_STRIDE(i) ((((((i) << 1) + (i)) << 3) - (i)) << 2) - (i)

#define EMIT_CODED(code_expr) { asm volatile(""); {  \
    int packet_code; \
    register char *base asm("$6"); \
    register int code_off asm("$3"); \
    int idx; \
    int idx_off; \
    register void *prim asm("$5"); \
    void *packet; \
    void *ot; \
    packet_code = (code_expr); \
    base = (char *)&g_StatusIconPackets[0][0].sprite; \
    code_off = ((packet_code << 3) - packet_code) << 2; \
    prim = base - 8; \
     \
    idx = g_ActiveDrawSlot; \
    prim = (char *)(code_off + (int)prim); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    prim = (char *)(idx_off + (int)prim); \
    packet = (char *)(idx_off + code_off + (int)base); \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    ot = g_OtBufferTable[idx]; \
    s0 += s3; \
    AddPrim((u32 *)ot + 7, prim); \
 } }

/* The last repeated icon queues its packet before advancing the cursor. */
#define EMIT_BASE(base_sym, advanceBeforeQueue) { \
    char *base; \
    int idx; \
    void *prim; \
    void *packet; \
    base = (base_sym); \
    idx = g_ActiveDrawSlot; \
    prim = base - 8; \
    packet = (char *)(INDEX_STRIDE(idx) << 2); \
    prim = (char *)((int)packet + (int)prim); \
    packet = (char *)((int)packet + (int)base); \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    if (advanceBeforeQueue) s0 += s3; \
    AddPrim((u32 *)g_OtBufferTable[idx] + 7, prim); \
    if (!(advanceBeforeQueue)) s0 += s3; \
}

#define EMIT_BASE_LAST(base_sym) { \
    int idx; \
    char *base; \
    int idx_off; \
    register void *packet asm("$3"); \
    idx = g_ActiveDrawSlot; \
    base = (base_sym); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    packet = (char *)(idx_off + (int)base); \
    idx <<= 2; \
    base = base - 8; \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    AddPrim((u32 *)(*(void **)((int)g_OtBufferTable + idx)) + 7, (char *)(idx_off + (int)base)); \
}

void Draw_StatusIcons(void) {
    Combatant *s1;
    char *lastIconBase;
    int s0;
    int s2;
    int s3;
    s32 *s4;
    u8 mode;
    int bits;
    register int code asm("$5");

    s1 = g_ActiveActor;
    mode = g_MenuActiveMode;
    s0 = 0xF;
    s4 = &s1->stateFlags;
    if (mode - 1U < 2U) {
        s0 = 0x121;
    }
    if (mode < 2) {
        s2 = 0xC1;
    } else {
        s2 = 0xF;
    }
    if ((short)s0 == 0x121) {
        s3 = -0x10;
    } else {
        s3 = 0x10;
    }

    {
        int bits0;
        bits0 = s1->stateFlags & 0xC;
        if (bits0 != 0) {
            code = bits0 ^ 0xC;
            code = code < 1;
            code <<= 2;
            EMIT_CODED(code);
        }
    }

    {
        int tmp;
        tmp = s1->stateFlags;
        bits = tmp & 0x30;
    }
    if (bits != 0) {
        code = 1;
        if (bits == 0x30) {
            code = 5;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *s4;
        bits = tmp & 0xC0;
    }
    if (bits != 0) {
        code = 2;
        if (bits == 0xC0) {
            code = 6;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *s4;
        bits = tmp & 3;
    }
    if (bits != 0) {
        code = 3;
        if (bits == 3) {
            code = 7;
        }
        EMIT_CODED(code);
    }

    if (*s4 & 0x1000) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][8].sprite, 1);
    }
    if (*s4 & 0x100) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][9].sprite, 1);
    }
    if (*s4 & 0x400) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][10].sprite, 1);
    }
    if (*s4 & 0x800) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][11].sprite, 0);
    }
    if (*s4 & 0x200) {
        lastIconBase = (char *)&g_StatusIconPackets[0][12].sprite;
        EMIT_BASE_LAST(lastIconBase);
    }
}

/* Active menu window mode and its on-screen bounds.
 * g_MenuActiveMode is private state of this pair. */

u8 g_MenuActiveMode;
u16 g_MenuActiveWindowYOffset;

void Battle_SetWindowBoundsByMode(int mode) {
    SaveWindowModeBounds bounds;
    u16 *entry;

    bounds = g_MenuWindowModeBounds;
    entry = &bounds.values[(u8)mode * 2];
    /* A record store: GCC 2.7 would let a scalar store pass the next load. */
    g_HudOrigin.x = entry[0];
    g_MenuActiveMode = mode;
    g_MenuActiveWindowYOffset = entry[1];
}

unsigned int Menu_GetActiveMode(void) {
    return g_MenuActiveMode;
}

void Script_SignalExit(void) {
    g_BattleEnemyEscapeFlag = 1;
}

/* Clamp the active combatant's gauges, draw the ATB and PE bars and the
 * status background, then the HP numbers and status icons. Each background
 * branch writes the whole tile; the compiler merges their common tails. */
void Battle_DrawActiveStatus(void)
{
    s16 maximumHP;
    s32 maximumGauge;
    s32 currentGauge;
    s32 atbWidth;
    s32 peWidth;
    s32 peProduct;
    RenderColorTilePacket *atbTrack;
    RenderGouraudQuad *atbFill;
    RenderSpritePacket *atbMarker;
    RenderSpritePacket *atbLabel;
    RenderGouraudQuad *peFill;
    RenderGouraudQuad *peRemainder;
    RenderSpritePacket *peLabel;
    BattleTileSlot *tiles;

    if (!(g_GameStateFlags & 2)) {
        g_ActiveActor = D_8009CD10->core;
    }
    if (g_ActiveActor->hpAlive >= 9001) {
        g_ActiveActor->hpAlive = 9000;
    }
    currentGauge = g_ActiveActor->exp_or_acc;
    maximumGauge = g_ActiveActor->maxAtk;
    if (maximumGauge < currentGauge) {
        g_ActiveActor->exp_or_acc = maximumGauge;
    } else if (currentGauge < 0) {
        g_ActiveActor->exp_or_acc = 0;
    }
    maximumHP = g_ActiveActor->maxHP;
    if (maximumHP < (s16)g_ActiveActor->curHP) {
        g_ActiveActor->curHP = maximumHP;
    }
    peProduct = (s16)(g_ActiveActor->exp_or_acc >> 16) * 0x38;
    atbWidth = (g_ActiveActor->hpAlive * 0x38) / 9000;
    peWidth = peProduct / (s16)(g_ActiveActor->maxAtk >> 16);
    if (g_GameStateFlags & 2) {
        atbTrack = &g_BattleAtbTracks[g_ActiveDrawSlot];
        atbTrack->x0 = g_HudOrigin.x + 8;
        atbTrack->y0 = g_HudOrigin.y + 4;
        atbFill = &g_BattleAtbGauges[g_ActiveDrawSlot];
        atbFill->x0 = g_HudOrigin.x + 8;
        atbFill->y0 = g_HudOrigin.y + 4;
        atbFill->x1 = g_HudOrigin.x + 8 + atbWidth;
        atbFill->y1 = g_HudOrigin.y + 4;
        atbFill->x2 = g_HudOrigin.x + 8;
        atbFill->y2 = g_HudOrigin.y + 7;
        atbFill->x3 = g_HudOrigin.x + 8 + atbWidth;
        atbFill->y3 = g_HudOrigin.y + 7;
        atbMarker = &g_BattleAtbMarkers[g_ActiveDrawSlot].sprite;
        atbMarker->x = g_BattleAtbGauges[g_ActiveDrawSlot].x1;
        atbMarker->y = g_BattleAtbGauges[g_ActiveDrawSlot].y1 - 2;
        atbLabel = &g_BattleAtbLabels[g_ActiveDrawSlot].sprite;
        atbLabel->x = g_HudOrigin.x + 0x44;
        atbLabel->y = g_HudOrigin.y + 3;
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 6, (u32 *)atbTrack);
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 5, (u32 *)&g_BattleAtbGauges[g_ActiveDrawSlot]);
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 4, (u32 *)&g_BattleAtbMarkers[g_ActiveDrawSlot]);
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 4, (u32 *)&g_BattleAtbLabels[g_ActiveDrawSlot]);
    }
    if (Menu_GetItemContextFlag() & 2) {
        peFill = &g_BattlePeGauges[g_ActiveDrawSlot][0];
        peFill->x0 = g_HudOrigin.x + 8;
        peFill->y0 = g_HudOrigin.y + 0x19;
        peFill->x1 = g_HudOrigin.x + 8 + peWidth;
        peFill->y1 = g_HudOrigin.y + 0x19;
        peFill->x2 = g_HudOrigin.x + 8;
        peFill->y2 = g_HudOrigin.y + 0x1C;
        peFill->x3 = g_HudOrigin.x + 8 + peWidth;
        peFill->y3 = g_HudOrigin.y + 0x1C;
        peRemainder = &g_BattlePeGauges[g_ActiveDrawSlot][1];
        peRemainder->x0 = g_BattlePeGauges[g_ActiveDrawSlot][0].x1;
        peRemainder->y0 = g_HudOrigin.y + 0x19;
        peRemainder->x1 = (u16)g_BattlePeGauges[g_ActiveDrawSlot][0].x1 + 0x38 - peWidth;
        peRemainder->y1 = g_HudOrigin.y + 0x19;
        peRemainder->x2 = g_BattlePeGauges[g_ActiveDrawSlot][0].x1;
        peRemainder->y2 = g_HudOrigin.y + 0x1C;
        peLabel = &g_BattlePeLabels[g_ActiveDrawSlot].sprite;
        peRemainder->x3 = (u16)g_BattlePeGauges[g_ActiveDrawSlot][0].x1 + 0x38 - peWidth;
        peRemainder->y3 = g_HudOrigin.y + 0x1C;
        peLabel->x = g_HudOrigin.x + 0x44;
        peLabel->y = g_HudOrigin.y + 0x18;
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 5, (u32 *)peFill);
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 5, (u32 *)&g_BattlePeGauges[g_ActiveDrawSlot][1]);
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 4, (u32 *)&g_BattlePeLabels[g_ActiveDrawSlot]);
        if (g_GameStateFlags & 2) {
            tiles = g_BattleStatusBackgroundTiles;
            tiles[g_ActiveDrawSlot].tile.x0 = g_HudOrigin.x;
            tiles[g_ActiveDrawSlot].tile.y0 = g_HudOrigin.y;
            tiles[g_ActiveDrawSlot].tile.w = 0x54;
            tiles[g_ActiveDrawSlot].tile.h = 0x20;
        } else {
            tiles = g_BattleStatusBackgroundTiles;
            tiles[g_ActiveDrawSlot].tile.x0 = g_HudOrigin.x;
            tiles[g_ActiveDrawSlot].tile.y0 = g_HudOrigin.y + 7;
            tiles[g_ActiveDrawSlot].tile.w = 0x54;
            tiles[g_ActiveDrawSlot].tile.h = 0x19;
        }
    } else if (g_GameStateFlags & 2) {
        tiles = g_BattleStatusBackgroundTiles;
        tiles[g_ActiveDrawSlot].tile.x0 = g_HudOrigin.x;
        tiles[g_ActiveDrawSlot].tile.y0 = g_HudOrigin.y;
        tiles[g_ActiveDrawSlot].tile.w = 0x54;
        tiles[g_ActiveDrawSlot].tile.h = 0x19;
    } else {
        tiles = g_BattleStatusBackgroundTiles;
        tiles[g_ActiveDrawSlot].tile.x0 = g_HudOrigin.x;
        tiles[g_ActiveDrawSlot].tile.y0 = g_HudOrigin.y + 7;
        tiles[g_ActiveDrawSlot].tile.w = 0x54;
        tiles[g_ActiveDrawSlot].tile.h = 0x12;
    }
    AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 7, (u32 *)&g_BattleStatusBackgrounds[g_ActiveDrawSlot]);
    Battle_DrawEnemyHP(g_ActiveActor->maxHP, g_ActiveActor->curHP);
    Draw_StatusIcons();
}

/* Digit sprites of the current draw buffer. Every use indexes again: the
 * byte stores to the packets make GCC reload the draw slot. */
#define MAX_HP_DIGIT(i) (&g_BattleMaxHpDigitSlots[g_ActiveDrawSlot][i].sprite)
#define CURRENT_HP_DIGIT(i) (&g_BattleHpDigitSlots[g_ActiveDrawSlot][i].sprite)

/* Draw current/maximum HP from reversed decimal digits and the HP label.
 * The maximum is drawn right to left after a slash, the current value is
 * right-aligned against it; flashing digits mark the status flags. */
void Battle_DrawEnemyHP(s16 maximum, s16 current)
{
    u8 digits[8];
    s16 y;
    s16 x;
    s8 n;
    s8 last;
    s8 missing;
    s16 quotient;
    u8 pulse;
    RenderSpritePacket *label;

    y = g_HudOrigin.y + 11;
    n = 0;
    for (;;) {
        quotient = maximum / 10;
        digits[n] = maximum - quotient * 10;
        maximum = quotient;
        if (maximum == 0)
            break;
        n++;
    }
    n++;
    digits[n] = 10;
    last = n;
    x = g_HudOrigin.x - n * 6 + 56;
    for (; n >= 0; n--) {
        MAX_HP_DIGIT(n)->u = digits[n] * 8;
        MAX_HP_DIGIT(n)->v = 0xE8;
        MAX_HP_DIGIT(n)->x = x;
        MAX_HP_DIGIT(n)->y = y;
        if (!(((Combatant *)D_8009CD10->core)->stateFlags & 0x800) || n == last) {
            MAX_HP_DIGIT(n)->color.bytes.r = 128;
            MAX_HP_DIGIT(n)->color.bytes.g = 128;
            MAX_HP_DIGIT(n)->color.bytes.b = 128;
        } else {
            pulse = g_BattleFrameCounter & 31;
            if (pulse >= 17)
                pulse = 32 - pulse;
            MAX_HP_DIGIT(n)->color.bytes.r = pulse * 6 + 32;
            MAX_HP_DIGIT(n)->color.bytes.g = pulse + 96;
            MAX_HP_DIGIT(n)->color.bytes.b = 128;
        }
        x += 6;
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 5, (u32 *)&g_BattleMaxHpDigits[g_ActiveDrawSlot][n]);
    }

    x = g_HudOrigin.x - (last * 2 + 1) * 6 + 62;
    n = 0;
    for (;;) {
        quotient = current / 10;
        digits[n] = current - quotient * 10;
        current = quotient;
        if (current == 0)
            break;
        n++;
    }
    missing = last - n - 1;
    if (missing > 0)
        x += missing * 6;
    for (; n >= 0; n--) {
        CURRENT_HP_DIGIT(n)->u = digits[n] * 8;
        CURRENT_HP_DIGIT(n)->v = 0xE8;
        CURRENT_HP_DIGIT(n)->x = x;
        CURRENT_HP_DIGIT(n)->y = y;
        if (((Combatant *)D_8009CD10->core)->stateFlags & 0x400) {
            pulse = g_BattleFrameCounter & 31;
            if (pulse >= 17)
                pulse = 32 - pulse;
            CURRENT_HP_DIGIT(n)->color.bytes.r = pulse * 8;
            CURRENT_HP_DIGIT(n)->color.bytes.g = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.b = pulse * 6 + 32;
        } else {
            CURRENT_HP_DIGIT(n)->color.bytes.r = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.g = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.b = 128;
        }
        x += 6;
        AddPrim(D_800B08F8.ordering[g_ActiveDrawSlot] + 5, (u32 *)&g_BattleHpDigits[g_ActiveDrawSlot][n]);
    }

    /* Place the HP label after the number pair. */
    {
        s32 frame = g_ActiveDrawSlot;
        RenderSpritePacket *sprites = &g_BattleHpLabels[0].sprite;

        /* Step the sprite pointer by whole packets. */
        label = (RenderSpritePacket *)((RenderPagedSpritePacket *)sprites + frame);
        label->x = g_HudOrigin.x + 0x44;
        label->y = g_HudOrigin.y + 0xE;
        AddPrim(D_800B08F8.ordering[frame] + 4, (u32 *)&g_BattleHpLabels[frame]);
    }
}

void Render_AnimationFrame(void) {
    int size;
    int width;
    int frame;
    RenderAnimActor *early_actor;
    RenderAnimActor *late_actor;
    RenderAnimPlayer *player;
    int draw_index;
    int delta;
    int scaled_frame;
    /* Preserves the original GCC 2.7.2 stack frame. */
    char frame_work[12];
    int animation_id;
    int zero_arg;
    int x;
    int y;
    int z;

    frame = g_BattleAttackAnimFrame;
    if (frame < 10U) {
        early_actor = (RenderAnimActor *)g_ActiveActor;
        if ((early_actor->modeFlags & 0x30) == 0x10) {
            scaled_frame = (frame << 2) + frame;
            size = scaled_frame << 18;
        } else {
            size = ((early_actor->frameData->frameCount << 16) * frame) / 10U;
        }
        width = g_BattleAttackAnimFrame << 6;
    } else {
        late_actor = (RenderAnimActor *)g_ActiveActor;
        size = 0xC80000;
        if ((late_actor->modeFlags & 0x30) != 0x10) {
            size = late_actor->frameData->frameCount << 16;
        }
        frame = 10;
        width = (g_BattleAttackAnimFrame << 4) + frame;
    }

    animation_id = 0x457;
    if (g_BattleAttackAnimFrame == 0) {
        player = (RenderAnimPlayer *)g_PlayerEntity;
        x = player->x;
        y = player->y;
        z = player->z;
        zero_arg = 0;
        Asset_Find08Alt(animation_id, zero_arg, x, y, z);
    }

    Render_SetGteScreenOffset();

    draw_index = g_ActiveDrawSlot;
    delta = Render_DrawWireframeModel(g_GameState.room_geometry_table,
                                      ((RenderAnimPlayer *)g_PlayerEntity)->decoderData,
                                      size, width & 0xFFFE,
                                      g_OtBufferTable[draw_index] + 8,
                                      g_RenderAnimBufferBase[draw_index] +
                                           g_DrawPacketOffset);
    g_DrawPacketOffset += delta;

    Render_ResetGteScreenOffset();
    g_BattleAttackAnimFrame++;
}

/* Status symbol templates (three 18-byte glyph strips the status primitive
 * flow copies) and the per-mode window bounds Battle_SetWindowBoundsByMode reads:
 * the data that sat between this module's rodata as main/rodata_0015FC.
 * They are defined after every user, so the users still copy them as the
 * byte-aligned objects their declarations describe; GCC emits them after the
 * status primitive constants and before the jump tables, which it writes at
 * the end of the file, as retail has them. */
const StatusSymbolTemplate g_BattleStatusSymbolUv = {{
    0x00, 0x00, 0x00, 0xF8, 0xD4, 0xF0, 0x00, 0x00, 0x90,
    0xE8, 0x28, 0xF8, 0xB4, 0xEF, 0x50, 0xF8, 0x58, 0xE8,
}};
const StatusSymbolTemplate g_BattleStatusSymbolSize = {{
    0x00, 0x00, 0x28, 0x07, 0x14, 0x07, 0x00, 0x00, 0x40,
    0x07, 0x28, 0x07, 0x1C, 0x07, 0x30, 0x07, 0x38, 0x07,
}};
const StatusSymbolTemplate g_BattleStatusSymbolOffset = {{
    0x00, 0x00, 0x14, 0x08, 0x1E, 0x08, 0x00, 0x00, 0x08,
    0x08, 0x14, 0x08, 0x1A, 0x08, 0x10, 0x08, 0x0C, 0x08,
}};
/* X and Y per mode: (221, 177), (15, 177), (15, 15), (221, 15). */
const SaveWindowModeBounds g_MenuWindowModeBounds = {{
    221, 177, 15, 177, 15, 15, 221, 15,
}};
