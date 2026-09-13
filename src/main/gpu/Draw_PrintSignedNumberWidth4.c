/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "common.h"
#include "pe1/draw_state.h"

void Draw_AllocSprite(int arg0);
void Draw_EmitDigitSprite(int digit);

void Draw_PrintSignedNumberWidth4(int value) {
    int width = 4;
    int place = 1;
    int i;
    int x;
    int y;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
    } else {
        if (value <= 0) {
            goto digits;
        }
        sprite = 0x89;
    }
    Draw_AllocSprite(sprite);
    width = 3;
    x = g_DrawSpriteX;
    y = g_DrawSpriteY;
    g_DrawSpriteX = x + 5;
    g_DrawSpriteY = y;
digits:
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}
