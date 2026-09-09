#ifndef PE1_PSYQ_TIM_H
#define PE1_PSYQ_TIM_H

#include "pe1/psyq_gpu.h"

/* The length includes this block's length word, rectangle and pixel bytes. */
typedef struct TimBlock {
    int length;
    RECT rect;
    int pixels[1];
} TimBlock;

typedef struct TimFile {
    int magic;
    int flags;
    TimBlock first_block;
} TimFile;

int LoadImage(RECT *rect, void *pixels);
int *Gpu_LoadTimImage(TimFile *tim);
/* Existing project symbols: CLUT rect, image rect, image data, CLUT data. */
RECT *Str_GetTableEntryA(TimFile *tim);
RECT *Widget_GetDataPtr(TimFile *tim);
int *Str_GetTableEntryB(TimFile *tim);
int *Str_GetTableEntryC(TimFile *tim);

#endif
