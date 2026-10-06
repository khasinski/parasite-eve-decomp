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

#endif
