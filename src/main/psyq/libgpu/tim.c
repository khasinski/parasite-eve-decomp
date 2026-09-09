#include "pe1/psyq_tim.h"

int *Gpu_LoadTimImage(TimFile *tim) {
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

RECT *Str_GetTableEntryA(TimFile *tim) {
    if (tim->flags & 8) {
        return &tim->first_block.rect;
    }
    return 0;
}

RECT *Widget_GetDataPtr(TimFile *tim) {
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

int *Str_GetTableEntryB(TimFile *tim) {
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

int *Str_GetTableEntryC(TimFile *tim) {
    if (tim->flags & 8) {
        return tim->first_block.pixels;
    }
    return 0;
}
