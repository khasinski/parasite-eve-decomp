/* Game-side TIM container helpers: upload a TIM's image and optional CLUT,
 * and return its CLUT/image rectangles and pixel pointers. No Psy-Q
 * signature of any SDK version matches these bytes; they are linked as the
 * last game object, directly before the LIBC veneers. */
#include "pe1/asset_tim.h"

int *Asset_LoadTimImage(TimFile *tim) {
    TimBlock *clut;
    TimBlock *image;
    int *pixels;

    clut = 0;
    if ((tim->flags & 8) != 0) {
        clut = &tim->first_block;
        image = (TimBlock *)((char *)clut + clut->length);
    } else {
        image = &tim->first_block;
    }

    pixels = image->pixels;
    LoadImage(&image->rect, pixels);
    if (clut != 0) {
        LoadImage(&clut->rect, clut->pixels);
    }
    return pixels;
}

RECT *Asset_GetTimClutRect(TimFile *tim) {
    if (tim->flags & 8) {
        return &tim->first_block.rect;
    }
    return 0;
}

RECT *Asset_GetTimImageRect(TimFile *tim) {
    TimBlock *image;
    register int length asm("$3");

    if (tim->flags & 8) {
        length = tim->first_block.length;
        image = &tim->first_block;
        image = (TimBlock *)((char *)image + length);
    } else {
        image = &tim->first_block;
    }
    return &image->rect;
}

int *Asset_GetTimImagePixels(TimFile *tim) {
    TimBlock *image;
    register int length asm("$3");

    if (tim->flags & 8) {
        length = tim->first_block.length;
        image = &tim->first_block;
        image = (TimBlock *)((char *)image + length);
    } else {
        image = &tim->first_block;
    }
    return image->pixels;
}

int *Asset_GetTimClutPixels(TimFile *tim) {
    if (tim->flags & 8) {
        return tim->first_block.pixels;
    }
    return 0;
}
