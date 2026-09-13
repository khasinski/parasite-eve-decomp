#ifndef PE1_FIELD_ANIM_H
#define PE1_FIELD_ANIM_H

#include "common.h"

typedef int (*FieldAnimTaskCallback)(int mode, void *state);

typedef struct FieldAnimTaskSlot {
    u32 control;
    char *start;
    char *end;
} FieldAnimTaskSlot;

extern FieldAnimTaskSlot *D_800F33E0;
int func_800CE560(char *out, int stride, int count, FieldAnimTaskCallback callback);
void *func_800CE610(char *list);

typedef struct FieldAnimEmitter {
    int angle;
} FieldAnimEmitter;

/* Prefix of the 16-byte payload allocated by this emitter. */
typedef struct FieldAnimEmittedPoint {
    s16 position[3];
    s16 state;
    s16 value08;
    s16 angle;
    s16 phase;
} FieldAnimEmittedPoint;

extern u8 *D_800F32D8;
extern s16 D_800E220C[];
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
int func_800D9A8C(int mode, void *state);
int FieldEng_PointEmitter(int mode, FieldAnimEmitter *state);

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

typedef struct FieldAnimBurstWindow {
    u8 unused_00[0x10];
    FieldAnimPointTriple point;
    u8 unused_16[0xA];
    FieldAnimPointTriple offset;
} FieldAnimBurstWindow;

typedef struct FieldAnimBurstHeader {
    u8 unused_00[3];
    u8 mode;
    u8 scale[2];
    u8 duration[2];
    u8 origin_x[2];
    u8 origin_y[2];
    u8 origin_z[2];
} FieldAnimBurstHeader;

typedef union FieldAnimBurstData {
    FieldAnimBurstHeader header;
    u8 bytes[0x30];
} FieldAnimBurstData;

extern FieldAnimInterleavedState D_800E2260;
extern FieldAnimPointState D_800E2818;
extern FieldAnimPointTriple D_800E27F8;

#endif
