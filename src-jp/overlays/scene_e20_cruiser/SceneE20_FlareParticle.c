#include "pe1/field_texture_tables.h"
#include "pe1/scene_e20_flare_draw.h"

/* Mode 1 ages the flare and, while it falls, moves and damps it and
 * bounces it off the floor; mode 2 draws a spinning glow with a streak,
 * a pulsing glow pair, or a fading spark. The two palette selections of
 * the spinning glow add a conditional offset to the palette base
 * (`base + (cond ? 8 : 4)`), each from its own palette kind read.
 * The two view-matrix loads are the PSY-Q SetRotMatrix/SetTransMatrix
 * inline transfers (gte_ldrotmatrix/gte_ldtransmatrix in pe1/gte.h). */
int SceneE20_FlareParticle(int mode, SceneE20Particle *p)
{
    GteRotation rotation;
    RenderColor color;
    RenderColor streakColor;
    RenderColor glowColor;
    int fall;
    int bounce;
    int scale;
    int angle;
    int glow;
    int time;
    int state;

    streakColor = g_SceneE20FlareStreakColor;
    glowColor = g_SceneE20FlareGlowColor;
    switch (mode) {
    case 1:
        switch (p->kind) {
        case 0:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            if (p->timer < 0x10) break;
            return 1;
        case 2:
            p->timer++;
            p->position.x += p->velocity.x;
            p->position.y += p->velocity.y;
            p->position.z += p->velocity.z;
            p->velocity.x = p->velocity.x * 59 / 60;
            p->velocity.z = p->velocity.z * 59 / 60;
            fall = (u16)p->velocity.y + 1;
            p->velocity.y = fall;
            if (p->position.y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->velocity.y = bounce;
            }
            if (p->timer < 0x14) break;
            return 1;
        }
        break;
    case 2:
        state = p->kind;
        switch (state) {
        case 0: {
            int kind;
            int palette;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            angle = p->timer << 6;
            scale = rsin(angle);
            rotation.x = p->velocity.x;
            rotation.y = p->velocity.y;
            rotation.z = p->velocity.z;
            rotation.flags = 1;
            gte_ldrotmatrix(D_800BCA64.value);
            gte_ldtransmatrix(D_800BCA64.value);
            FieldFx_SampleColorTrack(g_SceneE20FlareColorTrack, &color, p->timer);
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind] + ((kind == 4 && g_FieldFxAltPaletteFlag != 0) ? 8 : 4);
            FieldFx_DrawShapeQuads(&p->position, &rotation, scale * 3, scale * 3, 4,
                          GetClut(0, palette), 1, 0x80, &color);
            if (p->timer < 8) {
                int width = (p->timer / 2) << 5;
                int top = 0x40;
                int streakKind;
                glow = rcos(angle) / 32;
                FieldFx_SampleColorTrack(g_SceneE20FlareColorTrack, &color, p->timer << 1);
                streakKind = g_FieldFxSpriteParams.palette;
                palette = g_FieldFxPaletteTable[streakKind] + ((streakKind == 4 && g_FieldFxAltPaletteFlag != 0) ? 7 : 3);
                FieldFx_DrawRotatedQuad(&p->position, &rotation, 500, 0x168, width, top, 0x1F, 0x1F,
                              GetClut(0, palette), &streakColor, &streakColor,
                              (s16)glow, 1);
            }
            break;
        }
        case 1: {
            int kind;
            int palette;
            time = p->timer;
            angle = time << 6;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = p->timer * 32;
            rotation.flags = 1;
            g_FieldFxSpriteParams.parameter00 = 0x20;
            g_FieldFxSpriteParams.parameter02 = 2;
            g_FieldFxSpriteParams.extent_x = 0x20;
            g_FieldFxSpriteParams.extent_y = 0x20;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[3]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 0;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 4;
            FieldFx_DrawShapeQuads(&p->position, 0, 0x2000, 0x2000,
                          (s16)g_FieldFxSpriteParams.parameter02 * (p->timer / 2) + 0x60,
                          GetClut(0, palette), state, 0x80, 0);
            scale = rcos(angle) / 2 + 0x800;
            glow = rsin(time << 7) / 32;
            {
                int tpage = g_FieldFxTpageTable[g_FieldFxTpageSlots[11]];
                g_FieldFxSpriteParams.palette = 3;
                g_FieldFxSpriteParams.parameter06 = 1;
                g_FieldFxSpriteParams.tpage = tpage;
            }
            kind = g_FieldFxSpriteParams.palette;
            palette = g_FieldFxPaletteTable[kind];
            if (kind == 4 && g_FieldFxAltPaletteFlag != 0) palette += 9;
            else palette += 5;
            FieldFx_DrawShapeQuads(&p->position, 0, scale * 4, scale * 4, 6,
                          GetClut(0, palette), 3, glow, &glowColor);
            break;
        }
        case 2:
            gte_ldrotmatrix(D_800BCA64.value);
            gte_ldtransmatrix(D_800BCA64.value);
            FieldFx_SampleColorTrack(g_SceneE20FlareColorTrack, &color, (p->timer << 4) / 20);
            FieldFx_DrawPointTile(&p->position, (unsigned char *)&color, 0x80, 1);
            break;
        }
        break;
    }
    return 0;
}
