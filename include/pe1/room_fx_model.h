#ifndef PE1_ROOM_FX_MODEL_H
#define PE1_ROOM_FX_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"

struct RoomFxModelOwner;

/* Room model geometry that rides along a guide line next to its owner's
 * model: the line runs from (line_x0, line_z0) to (line_x1, line_z1). */
typedef struct RoomFxModelGeom {
    /* 0x00 */ u8 pad00[0x2C];
    /* 0x2C */ GteShortVector rotation;
    /* 0x34 */ GteMatrix matrix;
    /* 0x54 */ u8 pad54[0xC];
    /* 0x60 */ struct RoomFxModelOwner *owner;
    /* 0x64 */ u8 pad64[0x58];
    /* 0xBC */ u16 line_x0;
    /* 0xBE */ u16 line_z0;
    /* 0xC0 */ u16 line_x1;
    /* 0xC2 */ u16 line_z1;
} RoomFxModelGeom;

typedef struct RoomFxModelOwner {
    /* 0x000 */ u8 pad00[0x16];
    /* 0x016 */ s16 matrix_count;
    /* 0x018 */ u8 pad18[0x198];
    /* 0x1B0 */ void *animation;
    /* 0x1B4 */ RoomFxModelGeom geom;
} RoomFxModelOwner;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxModelOwner, geom) == 0x1B4, room_fx_model_geom_offset);

int abs(int value);
int Gte_ISqrt(int value);
int Gte_Atan2(int y, int x);
int rsin(int angle);
int rcos(int angle);
void func_80039B74(RoomFxModelGeom *geom, void *animation, int count, int mode);

#endif
