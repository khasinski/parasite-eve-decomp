/* CC1_FLAGS: -fno-schedule-insns */

#include "pe1/psyq_gpu.h"

#include "room_m273.h"

typedef struct {
    GteShortVector *position;
} RoomM273PaletteSpriteState;

int RoomM273_PulseSpriteCallback(int mode, RoomM273PaletteSpriteState *state) {
    register int frame asm("$3");
    int size, shade, kind, palette;
    u16 clut;
    int sample, sizeSample, sizeOffset, shadeOffset;
    GteRotation *zeroRotation;
    char stack_pad[8];

    if (mode == 1) {
        if (g_FieldFxCurrentAge >= 16) return 1;
    } else if (mode == 2) {
        frame = g_FieldFxCurrentAge - 1;
        sizeOffset = (frame << 8) & 0x3F00;
        shadeOffset = (frame << 9) & 0x3E00;
        sizeSample = ((RoomM273TrigEntry *)&((char *)rcossin_tbl)[sizeOffset])->low;
        asm volatile("" : : "r"(sizeSample));
        sample = *(int *)&((char *)rcossin_tbl)[shadeOffset];
        frame = 4;
        asm volatile("" : "=r"(sample) : "0"(sample));
        size = sizeSample * 2 + 4096;
        shade = (s16)sample >> 5;
        kind = g_FieldFxSpriteParams.palette;
        palette = g_FieldFxPaletteTable[kind];
        if (kind == frame && g_FieldFxAltPaletteFlag) {
            palette += 8;
        } else {
            palette += 4;
        }
        clut = GetClut(0, palette);
        zeroRotation = 0;
        asm volatile("" : "=r"(zeroRotation) : "0"(zeroRotation));
        FieldFx_DrawShapeQuads(state->position, zeroRotation, size, size, 100,
                       clut, 1, shade, 0);
    }
    return 0;
}
