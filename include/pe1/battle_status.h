#ifndef PE1_BATTLE_STATUS_H
#define PE1_BATTLE_STATUS_H

#include "common.h"
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

/* Texture-page command followed by the ammunition icon sprite. */
typedef struct BattleGaugePrim {
    RenderTexturePagePacket texture_page;
    RenderSpritePacket sprite;
} BattleGaugePrim;

extern BattleStatusLinePrim D_8009E358[6];
extern int g_ActiveDrawSlot;
extern BattleStatusMarkerBody D_8009E888[2];
extern u8 D_8009E8B8[2][0x38];
extern BattleGaugePrim D_8009E768[2];
extern u8 D_8009E7A0[2][0x70];

void AddPrim(unsigned int *orderingEntry, unsigned int *primitive);
void Battle_LayoutStatusPrimRow(int bottomY);
void Battle_DrawStatusValue(int value, int yOffset);
void Battle_DrawDecimalNumber(void *buffer, s16 x, s16 y, s16 value, int mode);

PE1_STATIC_ASSERT(sizeof(BattleStatusLinePrim) == 0x10,
                  battle_status_line_prim_size);
PE1_STATIC_ASSERT(sizeof(BattleStatusMarkerBody) == 0x1C,
                  battle_status_marker_body_stride);
PE1_STATIC_ASSERT(PE1_OFFSETOF(BattleGaugePrim, sprite) == 8,
                  battle_gauge_sprite_offset);
PE1_STATIC_ASSERT(sizeof(BattleGaugePrim) == 0x1C,
                  battle_gauge_prim_stride);

#endif
