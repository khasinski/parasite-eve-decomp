#ifndef PE1_BATTLE_STATUS_H
#define PE1_BATTLE_STATUS_H

#include "pe1/battle.h"
#include "pe1/render_prim.h"

typedef struct BattleStatusLinePrim {
    u8 pad00[8];
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
} BattleStatusLinePrim;

typedef struct BattleStatusMarkerBody {
    u8 pad00[8];
    u16 x;
    u16 y;
    u8 pad0C[0x10];
} BattleStatusMarkerBody;

/* Texture-page command followed by a variable-size UI sprite. */
typedef struct BattleGaugePrim {
    RenderTexturePagePacket texture_page;
    RenderSpritePacket sprite;
} BattleGaugePrim;

extern BattleStatusLinePrim D_8009E358[6];
extern int g_ActiveDrawSlot;
extern BattleStatusMarkerBody D_8009E888[2];
extern u8 D_8009E8B8[2][0x38];
extern BattleGaugePrim D_8009E3B8[2][3];
extern BattleGaugePrim D_8009E460[2];
/* Status symbols use the same texture-page and sprite packet layout. */
extern BattleGaugePrim D_8009E730[2];
extern BattleGaugePrim D_8009E768[2];
extern u8 D_8009E7A0[2][0x70];

void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);
void Battle_LayoutStatusPrimRow(int bottomY);
void Battle_DrawTargetHighlight(void);
void Battle_DrawActiveStatus(void);
void Battle_DrawEnemyHP(s16 maximum, s16 current);
void Gpu_DrawStatusIcons(void);
int Menu_GetItemContextFlag(void);

/* Shared HUD packet arenas; interior symbols retain independent relocations. */
extern u8 D_8009E068[], D_8009E070[], D_8009E098[], D_8009E0C0[];
extern u8 D_8009E0F0[], D_8009E0F8[], D_8009E1D0[], D_8009E1D8[];
extern u8 D_8009E2F0[];
extern u8 D_8009E328[], D_800B00E8[], D_800B00F8[], D_800B00FA[];
extern u8 D_800B0130[], D_800B0140[], D_800B6928[];
/* End-of-battle panel; color bytes also have independent retail symbols. */
extern RenderTexturedQuad D_800BE9F0[2];
extern u8 D_800BE9F4[], D_800BE9F5[], D_800BE9F6[];
void Battle_DrawStatusValue(int value, int yOffset);
/* Returns the highest digit index (number of rendered digits minus one). */
int Battle_DrawDecimalNumber(void *buffer, s16 x, s16 y, s16 value, s16 mode);

PE1_STATIC_ASSERT(sizeof(BattleStatusLinePrim) == 0x10,
                  battle_status_line_prim_size);
PE1_STATIC_ASSERT(sizeof(BattleStatusMarkerBody) == 0x1C,
                  battle_status_marker_body_stride);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleGaugePrim, sprite) == 8,
                  battle_gauge_sprite_offset);
PE1_STATIC_ASSERT(sizeof(BattleGaugePrim) == 0x1C,
                  battle_gauge_prim_stride);

/* Floating panel renderer matching views: wide draw-slot declarations keep
 * absolute addressing under -G8; the packet index uses the retail GP load.
 * Separate initial/read views preserve the original base-address lifetime.
 */
extern s32 g_BattlePanelDrawSlotView[16] asm("D_8009CDDC");
extern s32 g_BattlePanelInitialDrawSlotView[16] asm("D_8009CDDC");
extern s32 g_BattlePanelPacketIndex asm("D_8009D230");
extern RenderSpritePacket D_800B01C8[];
/* Interior byte symbols of the first panel sprite, independently relocated. */
extern u8 D_800B01CC[], D_800B01CD[], D_800B01CE[];
extern u8 D_800B01D4[], D_800B01D5[];
void Battle_DrawStatusPanel(int mode, BattleStatusPanel *panel);


/* Target pointer drawn by Battle_BuildStatusPrimHeader: a flat-shaded
 * triangle (POLY_F3 layout) plus two flat lines per draw slot that join the
 * pointer to the status panel. */
typedef struct BattleStatusPointerPrim {
    u32 tag;
    u8 r, g, b, code;
    s16 x0, y0, x1, y1, x2, y2;
} BattleStatusPointerPrim;

extern RenderLinePacket D_8009E498[2][2];
extern BattleStatusPointerPrim D_8009E4D8[2];
/* Frame counter; drives the pointer's rotation. */
extern u32 D_8009D250;

void SetRotMatrix(GteMatrix *matrix);
GteMatrix *TransMatrix(GteMatrix *matrix, GteVector *translation);
void SetTransMatrix(GteMatrix *matrix);
void RotTrans(const GteShortVector *v, GteVector *out, s32 *flag);

PE1_STATIC_ASSERT(sizeof(BattleStatusPointerPrim) == 0x14,
                  battle_status_pointer_prim_stride);

#endif
