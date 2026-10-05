#ifndef PE1_SIGNED_RECT_H
#define PE1_SIGNED_RECT_H

#include "common.h"

typedef struct PsxSignedRect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} PsxSignedRect;

PE1_STATIC_ASSERT(PE1_OFFSETOF(PsxSignedRect, w) == 4,
                  psx_signed_rect_width_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(PsxSignedRect, h) == 6,
                  psx_signed_rect_height_offset);
PE1_STATIC_ASSERT(sizeof(PsxSignedRect) == 8, psx_signed_rect_size);

#endif /* PE1_SIGNED_RECT_H */
