#ifndef PE1_GTE_TYPES_H
#define PE1_GTE_TYPES_H

#include "common.h"

typedef struct GteMatrix {
    s16 m[3][3];
    s32 t[3];
} GteMatrix;

/* Word transfers to GTE control registers pack successive matrix halfwords.
 * The fifth word includes the MATRIX alignment halfword, not translation.
 * Keep this view separate from the signed-halfword arithmetic view above.
 */
typedef struct GteMatrixWords {
    u32 r11_r12;
    u32 r13_r21;
    u32 r22_r23;
    u32 r31_r32;
    u32 r33_pad;
    s32 tx, ty, tz;
} GteMatrixWords;

PE1_STATIC_ASSERT(sizeof(GteMatrixWords) == sizeof(GteMatrix), gte_matrix_words_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GteMatrixWords, r33_pad) == 16,
                  gte_matrix_words_last_rotation_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GteMatrixWords, tx) == PE1_OFFSETOF(GteMatrix, t),
                  gte_matrix_words_translation_offset);

typedef struct GteShortVector {
    s16 x, y, z, pad;
} GteShortVector;

typedef struct GteVector {
    s32 x, y, z, pad;
} GteVector;

PE1_STATIC_ASSERT(sizeof(GteMatrix) == 32, gte_matrix_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(GteMatrix, t) == 20, gte_matrix_translation_offset);
PE1_STATIC_ASSERT(sizeof(GteShortVector) == 8, gte_short_vector_size);
PE1_STATIC_ASSERT(sizeof(GteVector) == 16, gte_vector_size);

typedef struct GteRotation {
    s16 x;
    s16 y;
    s16 z;
    s16 flags;
} GteRotation;

#endif
