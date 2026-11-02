/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* cc1 -G8 makes the small texture slot word a cheap address, so its
 * volatile read stays an absolute load. */

#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/gte_sine_table.h"
#include "pe1/psyq_gpu.h"

#include "room_m350_shared.h"
#include "RoomEffect_ConfiguredPulseCallback_externs.h"
#include "RoomEffect_ConfiguredPulseCallback_externs_1.h"

/* Signed high-half view avoids LHU followed by explicit sign extension. */

int RoomM350_AreaPlayerPulseCallback(int event, void *object)
{
    /* Match note: retail leaves sp+0x28..0x2F unused; original local unknown. */
    char frameGap[8];
    if (event == 1) {
        if (g_FieldFxCurrentAge >= 8) return 1;
    } else if (event == 2) {
        volatile RenderEffectParameters *state = &g_FieldFxSpriteParams;
        int unit = 32;
        s16 size;
        int paletteIndex;
        int sampleOffset;
        u16 palette;
        int frame;
        s16 mode;
        int clutX;
        unsigned short handle;
        asm("" : "=r"(state) : "0"(state));
        state->parameter00 = unit;
        paletteIndex = *(volatile u16 *)&g_FieldFxTpageSlot2;
        mode = 2;
        g_FieldFxSpriteParams.parameter02 = mode;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        g_FieldFxSpriteParams.extent_x = unit;
        g_FieldFxSpriteParams.extent_y = unit;
        palette = g_FieldFxTpageTable[paletteIndex];
        frame = g_FieldFxCurrentAge;
        clutX = 0;
        g_FieldFxSpriteParams.palette = mode;
        g_FieldFxSpriteParams.parameter06 = 0;
        g_FieldFxSpriteParams.parameter0A = 0;
        g_FieldFxSpriteParams.depth = unit;
        sampleOffset = ((unsigned int)(frame - 1) << 9) & 0x3E00;
        g_FieldFxSpriteParams.tpage = palette;
        size = *(short *)&((char *)rcossin_tbl)[sampleOffset];
        size = size * 2 + 4096;
        handle = GetClut(clutX, g_FieldFxPaletteEntry2);
        {
            int frame = g_FieldFxCurrentAge - 1;
            int phase = (frame >> 2) & 3;
            int *colors;
            unsigned int drawSampleOffset;
            drawSampleOffset = ((unsigned int)frame << 9) & 0x3E00;
            colors = g_RoomM350AreaPlayerPulseColor;
            FieldFx_DrawShapeQuads(object, 0, size, size,
                (s16)((RenderEffectParameters *)state)->parameter02 * phase + 192,
                handle, 1, rcossin_tbl[drawSampleOffset / 4].bits.cos >> 5, (struct RenderColor *)colors);
        }
    }
    return 0;
}
