/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"

extern int D_8009D13C;
extern int D_8009D140;
extern int D_8009D144;

void Draw_AllocSprite(int arg0);
void Draw_EmitDigitSprite(int digit);

void Draw_PrintTimeValue(int value, int flag) {
    int x;
    int y;

    if (value > 360000) {
        value = 359999;
    }
    D_8009D13C = flag != 0 ? 0x3A1C : 0x395D;
    D_8009D140 = flag != 0 ? 0xCC : 0x84;
    D_8009D144 = 0xA4;

    Draw_EmitDigitSprite(value / 36000);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 3600);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 600) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 60);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 10) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value);

    D_8009D13C = 0x395D;
    D_8009D140 = 0x84;
    D_8009D144 = 0xA4;
}
