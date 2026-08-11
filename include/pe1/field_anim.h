#ifndef PE1_FIELD_ANIM_H
#define PE1_FIELD_ANIM_H

#include "common.h"

typedef struct FieldAnimPointTriple {
    u16 x;
    u16 y;
    u16 z;
} FieldAnimPointTriple;

typedef struct FieldAnimPointData {
    u8 unused_00[3];
    u8 scale;
    s16 count;
    u16 unused_06;
    u16 x[16];
    u16 y[16];
    u16 z[16];
} FieldAnimPointData;

typedef struct FieldAnimPointState {
    FieldAnimPointTriple point;
    u8 unused_06[0x22];
    short scale;
} FieldAnimPointState;

typedef struct FieldAnimInterleavedState {
    FieldAnimPointTriple point;
    u8 unused_06[0xA];
    int extent_x;
    int extent_y;
    int extent_z;
    u8 unused_1C[0xC];
    short scale;
} FieldAnimInterleavedState;

typedef struct FieldAnimInterleavedWindow {
    u8 unused_00[0x26];
    FieldAnimPointTriple point;
} FieldAnimInterleavedWindow;

extern FieldAnimInterleavedState D_800E2260;
extern FieldAnimPointState D_800E2818;

#endif
