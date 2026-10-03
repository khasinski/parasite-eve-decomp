#ifndef PE1_FIELD_TRI_TEST_H
#define PE1_FIELD_TRI_TEST_H

#include "pe1/gte_types.h"

/* Scratchpad work area for the floor-triangle containment test: three edge
 * vectors, the point relative to each corner, and the OP cross products. */
typedef struct FieldTriScratch {
    GteVector edge[3];  /* 0x00 */
    GteVector rel[3];   /* 0x30 */
    GteVector cross[3]; /* 0x60 */
} FieldTriScratch;

#define FIELD_TRI_SCRATCH ((FieldTriScratch *)0x1F800000)

extern FieldTriScratch *D_800E2844;

/* Player entity; the 16.16 field position integer halves sit at 0x2A/0x2E/0x32. */
typedef struct FieldTriEntity {
    u8 pad_00[0x28];
    struct {
        int fraction : 16;
        int integer : 16;
    } pos[3];
} FieldTriEntity;

extern FieldTriEntity *D_8009D254;

int func_800C62DC(GteShortVector *point, GteShortVector *tri);
int func_800C689C(GteShortVector *tri);

#endif /* PE1_FIELD_TRI_TEST_H */
