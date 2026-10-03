#include "menu_memcard_image.h"
#include "menu_memcard_screen.h"

typedef void (*MemcardNodeStep)(MemcardImageNode *node);
typedef void (*MemcardNodeDraw)(MemcardImageNode *node, u32 *destination, s32 padding, s32 stride);

/* Background pixels follow the 20-byte image header (24-bit, 320 words per row). */
typedef struct MemcardBackground {
    u8 header[0x14];
    u32 pixels[1];
} MemcardBackground;

/*
 * Step every image node, restore the background under the area the nodes
 * cover now or covered last frame, then let each node draw itself into it.
 */
void func_8018F468(void) {
    MemcardImageNode *node;
    MemcardScreenBuffer *screen;
    MemcardBackground *background;
    MemcardNodeStep step;
    MemcardNodeDraw draw;
    RECT area;
    u32 *source;
    u32 *destination;
    s32 left;
    s32 top;
    s32 right;
    s32 bottom;
    s32 edge;
    s32 lower;
    s32 x;
    s32 offset;
    s32 skip;
    s32 width;
    s32 height;
    s32 y;
    s32 words;
    s32 row;
    s32 column;

    top = 0x7FFF;
    left = 0x7FFF;
    bottom = 0;
    right = 0;
    for (node = D_801D1370; node != 0; node = node->next) {
        step = node->draw;
        if (step != 0) {
            step(node);
        }
        if (node->x < left) {
            left = node->x;
        }
        if (node->y < top) {
            top = node->y;
        }
        if (right < node->x + node->width) {
            right = node->x + node->width;
        }
        if (bottom < node->y + node->height) {
            bottom = node->y + node->height;
        }
    }

    screen = D_801D11C4;
    if (screen->dirty.w > 0) {
        x = left;
        if (screen->dirty.x < left) {
            x = screen->dirty.x;
        }
        area.x = x;
        y = top;
        if (screen->dirty.y < top) {
            y = screen->dirty.y;
        }
        area.y = y;
        edge = right;
        if (right < screen->dirty.x + screen->dirty.w) {
            edge = screen->dirty.x + screen->dirty.w;
        }
        area.w = edge - x;
        lower = bottom;
        if (bottom < screen->dirty.y + screen->dirty.h) {
            lower = screen->dirty.y + screen->dirty.h;
        }
        area.h = lower - y;
    } else {
        area.w = right - left;
        area.x = left;
        area.y = top;
        area.h = bottom - top;
    }

    if ((width = area.w) > 0 && (height = area.h) > 0) {
        background = (MemcardBackground *)&D_80193254[D_80193258[0]];
        source = &background->pixels[((area.y - 20) * 320 + area.x) * 3 / 4];
        destination = (u32 *)D_801D11C4->overlayImage;
        words = width * 3 / 4;
        for (row = 0; row < height; row++) {
            for (column = 0; column < words; column++) {
                *destination++ = *source++;
            }
            source += 240 - words;
        }
        for (node = D_801D1370; node != 0; node = node->next) {
            draw = node->update;
            if (draw != 0) {
                offset = area.w * (node->y - area.y) + (node->x - area.x);
                skip = area.w - node->width;
                draw(node, (u32 *)D_801D11C4->overlayImage + offset * 3 / 4, skip * 3 / 4,
                     area.w * 3 / 4);
            }
        }
    }

    {
        MemcardScreenBuffer *current = D_801D11C4;

        current->dirty.w = right - left;
        current->dirty.x = left;
        current->dirty.y = top;
        current->dirty.h = bottom - top;
        current->overlay = area;
    }
}
