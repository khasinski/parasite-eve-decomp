#include "pe1/draw_wipe_rect.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Wipe-bar panel over the polygon `points` ((x, y) byte pairs ending in
 * 0xFF, followed by its edge list) or, without points, over the
 * width x height rectangle at the text cursor. A textured panel also
 * gets a 4-bit sprite between two texture-window resets. */
/* Texture-window packet for `area`, linked into the ordering table. */
#define DRAW_ADD_TEXTURE_WINDOW(packet, area)               \
    {                                                       \
        RECT *rect = (area);                                \
                                                            \
        DRAW_ALLOC_PACKET(packet, sizeof(GpuCmdPacket));    \
        if ((packet).word)                                  \
            SetTexWindow((packet).window, rect);            \
        DRAW_LINK_PACKET(packet);                           \
    }

void Draw_AllocColorGradient(int width, int height, u8 *points, int textured)
{
    DrawPacketAddress windowPacket, sprite, mode;
    RECT window;

    if (points) {
        u8 *cursor = points;
        u32 value;

        g_DrawVertexWritePtr = g_TextCursorStackTop;
        value = cursor[0];
        if (value < 0xFF) {
            u16 *end = g_TextCursorStackTop + 0x18;

            do {
                DRAW_PUSH_WIPE_VERTEX(value + g_TextCursorX,
                                      g_TextCursorY + cursor[1], end);
                cursor += 2;
                value = cursor[0];
            } while (value < 0xFF);
        }
        Draw_EmitWipeBar(cursor + 1, 0);
    } else {
        g_DrawVertexWritePtr = g_TextCursorStackTop;
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY,
                              g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY,
                              g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY + height,
                              g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY + height,
                              g_TextCursorStackTop + 0x18);
        Draw_EmitWipeBar(D_800930A8, 0);
    }

    if (textured) {
        window.x = 0;
        window.y = 0;
        window.w = 0;
        window.h = 0;
        DRAW_ADD_TEXTURE_WINDOW(windowPacket, &window);

        DRAW_ALLOC_PACKET(sprite, sizeof(RenderSpritePacket));
        if (sprite.word) {
            if (D_8009D10C)
                sprite.sprite->color.word = D_8009D114;
            else
                sprite.sprite->color.word = D_8009D110;
            sprite.sprite->tag.bytes.length = 4;
            sprite.sprite->color.bytes.code = 0x64;
        }
        sprite.sprite->u = 0;
        sprite.sprite->v = 0;
        sprite.sprite->y = g_TextCursorY;
        sprite.sprite->x = g_TextCursorX;
        sprite.sprite->clut = 0x391C;
        sprite.sprite->width = width;
        sprite.sprite->height = height;
        DRAW_LINK_PACKET(sprite);

        window.x = 0;
        window.y = 0;
        window.w = 0x20;
        window.h = 0x20;
        DRAW_ADD_TEXTURE_WINDOW(windowPacket, &window);
    }

    DRAW_ALLOC_PACKET(mode, sizeof(RenderDrawModePacket));
    if (mode.word)
        SetDrawMode(mode.bytes, 0, 0, 7);
    DRAW_LINK_PACKET(mode);
}
