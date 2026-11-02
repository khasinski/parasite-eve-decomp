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
void MenuCard_RedrawImageNodes(void) {
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
    for (node = (g_MemcardActiveNodeHead); node != 0; node = node->next) {
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

    screen = g_MemcardCurrentScreen;
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
        background = (MemcardBackground *)&g_MemcardImageBundle[g_MemcardImageOffsets[0]];
        source = &background->pixels[((area.y - 19) * 320 + area.x) * 3 / 4];
        destination = (u32 *)g_MemcardCurrentScreen->overlayImage;
        words = width * 3 / 4;
        for (row = 0; row < height; row++) {
            for (column = 0; column < words; column++) {
                *destination++ = *source++;
            }
            source += 240 - words;
        }
        for (node = (g_MemcardActiveNodeHead); node != 0; node = node->next) {
            draw = node->update;
            if (draw != 0) {
                offset = area.w * (node->y - area.y) + (node->x - area.x);
                skip = area.w - node->width;
                draw(node, (u32 *)g_MemcardCurrentScreen->overlayImage + offset * 3 / 4, skip * 3 / 4,
                     area.w * 3 / 4);
            }
        }
    }

    {
        MemcardScreenBuffer *current = g_MemcardCurrentScreen;

        current->dirty.w = right - left;
        current->dirty.x = left;
        current->dirty.y = top;
        current->dirty.h = bottom - top;
        current->overlay = area;
    }
}

void MenuCard_BlendImage(MemcardImageNode *node, u8 *destination, s32 padding) {
    register s32 width asm("$3") = node->width;
    s32 intensity = node->intensity;
    s32 words;
    s32 row;
    u8 *source;
    s32 height;
    s32 column;
    width = (width << 1) + width;
    if (width < 0) width += 3;
    words = width >> 2;

    {
        MemcardImage *image = node->image;
        height = node->height;
        source = (u8 *)(image + 1);
    }
    for (row = 0; row < height; row++) {
        for (column = 0; column < words; column++) {
            if (*(u32 *)source) {
                {
                    u8 *target = destination;
                    s32 old;
                    s32 input = *source;
                    s32 product;
                    s32 value;

                    product = input * intensity;
                    source++;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    s32 old;
                    s32 input = *source;
                    s32 product;
                    s32 value;

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    s32 old;
                    s32 input = *source;
                    s32 product;
                    s32 value;

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    input = product >> 8;
                    __asm__("" : "=r"(input), "=r"(destination) : "0"(input), "1"(destination));
                    value = input;
                    destination++;
                    if (value < old) value = old;
                    *target = value;
                }
                {
                    u8 *target;
                    s32 old;
                    s32 input = *source;
                    s32 product;

                    product = input * intensity;
                    source++;

                    target = destination;
                    old = *destination;
                    width = product >> 8;
                    destination++;
                    if (width < old) width = old;
                    *target = width;
                }
            } else { source += 4; destination += 4; }
        }
        destination += padding * 4;
    }
}

void MenuCard_BlendRotatedImage(MemcardImageNode *node, u8 *destination, s32 padding, s32 stride) {
    u8 *out = destination;
    s32 progress = node->targetY;
    MemcardImage *image = node->image;
    u8 *source;
    s32 intensity = node->intensity;
    s32 limit = progress + 24;
    s32 row;
    s32 columns;
    s32 column;
    s32 product;
    s32 eightyFour;
    s32 workspace[1];
    __asm__("" : : "m"(workspace[0]));
    source = (u8 *)(image + 1);
    if (60 - progress >= 0) {
        s32 offset;
        product = stride * (60 - progress);
        offset = product << 2;
        out += offset;
    }
    for (row = 0; row < 24; row++, limit--) {
        columns = 84;
        if (limit < 85) columns = limit;
        for (column = 0; column < columns; column++) {
            {
                s32 input = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
            {
                s32 input = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
            {
                s32 input = *source++;
                u8 *target = out;
                s32 old = *out;
                s32 value;
                product = input * intensity;
                input = product >> 8;
                __asm__("" : "=r"(input), "=r"(out) : "0"(input), "1"(out));
                value = input;
                out++;
                if (value < old) value = old;
                *target = value;
            }
        }
        eightyFour = 84;
        {
            s32 skip = eightyFour - columns;
            source += skip * 3;
            out += padding * 4 + skip * 3;
        }
    }
    image = node->image;
    __asm__("" : "=r"(image) : "0"(image) : "memory");
    source = &((u8 *)image)[0x17B1];
    {
        s32 offset = (progress << 1) + progress;
        out = destination + offset;
    }
    {
        s32 count = eightyFour - progress;
        row = 0;
        if (count > 0) {
            s32 remaining;
            columns = count;
            __asm__("" : "=r"(columns) : "0"(columns));
            remaining = 84;
            do {
                u8 *in = source;
                u8 *dst = out;
                for (column = 0; (remaining - progress < 25 ? column < remaining - progress : column < 24); column++) {
                    {
                        s32 input = in[0];
                        u8 *target = dst;
                        register s32 old asm("$8") = *dst;
                        s32 value;
                        product = input * intensity;
                        input = product >> 8;
                        __asm__("" : "=r"(input), "=r"(dst) : "0"(input), "1"(dst));
                        value = input;
                        dst++;
                        if (value < old) value = old;
                        *target = value;
                    }
                    {
                        s32 input = in[1];
                        u8 *target = dst;
                        register s32 old asm("$8") = *dst;
                        s32 value;
                        product = input * intensity;
                        input = product >> 8;
                        __asm__("" : "=r"(input), "=r"(dst) : "0"(input), "1"(dst));
                        value = input;
                        dst++;
                        if (value < old) value = old;
                        *target = value;
                    }
                    {
                        s32 input = in[2];
                        u8 *target = dst;
                        register s32 old asm("$8");
                        register s32 value asm("$3");
                        product = input * intensity;
                        __asm__("" : "=r"(dst), "=r"(target) : "0"(dst), "1"(target));
                        old = *dst;
                        value = product >> 8;
                        dst = target + 1;
                        if (value < old) value = old;
                        *target = value;
                    }
                    in -= 252;
                }
                source -= 3;
                out += stride * 4;
                row++; remaining--;
            } while (row < columns);
        }
    }
}

/* Matching debt: the workspace operand preserves the retail stack frame. */
MemcardImageNode *MenuCard_CreateImageNode(s32 kind) {
    s32 workspace[1];
    MemcardImageNode *node = g_MemcardFreeImageNodes;
    MemcardImageNode *tail = g_MemcardPendingNodeTail;
    MemcardImage *imageData;
    MemcardImagePreset *preset = &g_MemcardImagePresets[kind];
    __asm__("" : : "m"(workspace[0]));
    g_MemcardFreeImageNodes = node->next;
    node->next = 0;
    if (tail) { tail->next = node; g_MemcardPendingNodeTail = node; }
    else { g_MemcardPendingNodeTail = node; g_MemcardPendingNodeHead = node; }
    node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
    node->x = 0; node->y = 0; node->width = 0; node->height = 0;
    node->intensity = 0; node->targetY = 0; node->stepY = 0;
    node->parameter = 0; node->kind = 0; node->freePending = 0;
    imageData = (MemcardImage *)(g_MemcardImageBundle + g_MemcardImageOffsets[kind]);
    node->kind = kind;
    node->image = imageData;
    {
        MemcardImage *image = node->image;
        node->x = preset->x; node->y = preset->y;
        node->width = image->width * 2 / 3;
        node->height = node->image->height;
    }
    {
        s32 y = node->y;
        s32 parameter;
        parameter = preset->parameter;
        node->draw = g_MenuCardUpdateFadeCounterEntry; node->update = MenuCard_BlendImage;
        node->targetY = y; node->parameter = parameter;
    }
    node->stepY = -16;
    return node;
}

void MenuCard_CreateFixedImageNode(void) {
    s32 kind;
    MemcardImageNode *node = g_MemcardFreeImageNodes;
    register MemcardImageNode *tail;
    MemcardImage *imageData;
    register MemcardImagePreset *preset;
    {
        MemcardImageNode *next = node->next;
        kind = 4;
        g_MemcardFreeImageNodes = next;
    }
    tail = g_MemcardPendingNodeTail;
    preset = &g_MemcardImagePresets[4];
    node->next = 0;
    if (tail) { tail->next = node; g_MemcardPendingNodeTail = node; }
    else { g_MemcardPendingNodeTail = node; g_MemcardPendingNodeHead = node; }
    node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
    node->x = 0; node->y = 0; node->width = 0; node->height = 0;
    node->intensity = 0; node->targetY = 0; node->stepY = 0;
    node->parameter = 0; node->kind = 0; node->freePending = 0;
    imageData = (MemcardImage *)(g_MemcardImageBundle + g_MemcardImageOffsets[4]);
    node->kind = kind;
    node->image = imageData;
    {
        register MemcardImage *image asm("$3") = node->image;
        node->x = preset->x; node->y = preset->y;
        node->width = image->width * 2 / 3;
        image = node->image;
        node->height = image->height;
    }
    {
        register s32 parameter = preset->parameter;
        node->draw = g_MenuCardAdvanceFadeEntry;
        node->update = MenuCard_BlendImage;
        node->parameter = parameter;
    }
}

void MenuCard_CreateImageGroup(void) {
    {
        register s32 kind;
        MemcardImageNode *node = g_MemcardFreeImageNodes;
        register MemcardImageNode *tail;
        MemcardImage *imageData;
        register MemcardImagePreset *preset;
        {
            MemcardImageNode *next = node->next;
            kind = 7;
            g_MemcardFreeImageNodes = next;
        }
        tail = g_MemcardPendingNodeTail;
        preset = &g_MemcardImagePresets[7];
        node->next = 0;
        if (tail) { tail->next = node; g_MemcardPendingNodeTail = node; }
        else { g_MemcardPendingNodeTail = node; g_MemcardPendingNodeHead = node; }
        node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
        node->x = 0; node->y = 0; node->width = 0; node->height = 0;
        node->intensity = 0; node->targetY = 0; node->stepY = 0;
        node->parameter = 0; node->kind = 0; node->freePending = 0;
        imageData = (MemcardImage *)(g_MemcardImageBundle + g_MemcardImageOffsets[7]);
        node->kind = kind;
        node->image = imageData;
        {
            register MemcardImage *image asm("$3") = node->image;
            node->x = preset->x; node->y = preset->y;
            node->width = image->width * 2 / 3;
            image = node->image;
            node->height = image->height;
        }
        {
            register s32 parameter = preset->parameter;
            node->update = MenuCard_BlendImage;
            node->parameter = parameter;
        }
    }
    MenuCard_CreateImageNode(3);
    MenuCard_CreateFixedImageNode();
    MenuCard_CreateImageNode(5)->field14 = g_MenuCardInitFadeForNodeEntry;
    {
        register s32 kind;
        MemcardImageNode *node = g_MemcardFreeImageNodes;
        register MemcardImageNode *tail;
        MemcardImage *imageData;
        register MemcardImagePreset *preset;
        {
            MemcardImageNode *next = node->next;
            kind = 6;
            g_MemcardFreeImageNodes = next;
        }
        tail = g_MemcardPendingNodeTail;
        preset = &g_MemcardImagePresets[6];
        node->next = 0;
        if (tail) { tail->next = node; g_MemcardPendingNodeTail = node; }
        else { g_MemcardPendingNodeTail = node; g_MemcardPendingNodeHead = node; }
        node->draw = 0; node->update = 0; node->field14 = 0; node->image = 0;
        node->x = 0; node->y = 0; node->width = 0; node->height = 0;
        node->intensity = 0; node->targetY = 0; node->stepY = 0;
        node->parameter = 0; node->kind = 0; node->freePending = 0;
        imageData = (MemcardImage *)(g_MemcardImageBundle + g_MemcardImageOffsets[6]);
        node->kind = kind;
        node->image = imageData;
        {
            register MemcardImage *image asm("$3") = node->image;
            node->x = preset->x; node->y = preset->y;
            node->width = image->width * 2 / 3;
            image = node->image;
            node->height = image->height;
        }
        {
            register s32 parameter = preset->parameter;
            node->draw = g_MenuCardUpdateFadeDisplayEntry;
            node->update = MenuCard_BlendRotatedImage;
            node->parameter = parameter;
        }
    }
}
