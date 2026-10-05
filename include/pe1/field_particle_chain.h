#ifndef PE1_FIELD_PARTICLE_CHAIN_H
#define PE1_FIELD_PARTICLE_CHAIN_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_textured_chain_node.h"
#include "pe1/room_glow_orb.h"

typedef struct FieldChainRecord {
    /* 0x00 */ FieldChainLink *links;
    /* 0x04 */ s16 count;
    /* 0x06 */ s16 field06;
    /* 0x08 */ s16 depth;
    /* 0x0A */ s16 length;
    /* 0x0C */ s16 field0C;
    /* 0x0E */ s16 field0E;
    /* 0x10 */ u8 field10;
    /* 0x11 */ u8 field11;
    /* 0x12 */ u8 bend;
    /* 0x13 */ u8 pad13;
    /* 0x14 */ GteMatrix matrix;
} FieldChainRecord;

void OuterProduct0(void *matrix_column, void *vector, void *output);
int rand(void);

#endif
