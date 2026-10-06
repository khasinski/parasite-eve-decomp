#ifndef PE1_ROOM_FX_MODEL_H
#define PE1_ROOM_FX_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"

struct RoomFxModelOwner;

/* Room model geometry that rides along a guide line next to its owner's
 * model: the line runs from (line_x0, line_z0) to (line_x1, line_z1). */
typedef struct RoomFxModelGeom {
    /* 0x00 */ u8 pad00[0x28];
    /* 0x28 */ s16 mode;
    /* 0x2A */ u8 pad2A[2];
    /* 0x2C */ GteShortVector rotation;
    /* 0x34 */ GteMatrix matrix;
    /* 0x54 */ u8 pad54[0xC];
    /* 0x60 */ struct RoomFxModelOwner *owner;
    /* 0x64 */ u8 pad64[0x38];
    /* 0x9C */ u16 flags;
    /* 0x9E */ u8 pad9E[0x1E];
    /* 0xBC */ u16 line_x0;
    /* 0xBE */ u16 line_z0;
    /* 0xC0 */ u16 line_x1;
    /* 0xC2 */ u16 line_z1;
} RoomFxModelGeom;

typedef struct RoomFxModelOwner {
    /* 0x000 */ u8 pad00[4];
    /* 0x004 */ struct RoomFxModelOwner *next; /* field actor list */
    /* 0x008 */ u8 pad08[4];
    /* 0x00C */ u8 kind;
    /* 0x00D */ u8 subKind;
    /* 0x00E */ u8 pad0E[8];
    /* 0x016 */ s16 matrix_count;
    /* 0x018 */ u8 pad18[0x80];
    /* 0x098 */ u32 status;           /* 0x10: busy */
    /* 0x09C */ u8 pad9C[0x114];
    /* 0x1B0 */ void *animation;
    /* 0x1B4 */ RoomFxModelGeom geom;
} RoomFxModelOwner;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFxModelOwner, geom) == 0x1B4, room_fx_model_geom_offset);

int abs(int value);
int rsin(int angle);
int rcos(int angle);
void func_80039B74(RoomFxModelGeom *geom, void *animation, int count, int mode);

/* The mirror room controller (src/overlays/room_lib/RoomFx_MirrorModel.c),
 * linked by room_m017, room_m018, room_m021, room_m045, room_m102,
 * room_m151 and room_m319: it finds an actor by kind, binds a mirror model
 * to it across a guide line and redraws the reflection every frame. */
typedef struct RoomFxMirror {
    /* 0x00 */ u8 pad00[8];
    /* 0x08 */ RoomFxModelOwner *object;  /* carries the mirror geometry */
    /* 0x0C */ RoomFxModelOwner *target;  /* the reflected actor */
    /* 0x10 */ s16 lineX0;
    /* 0x12 */ s16 lineZ0;
    /* 0x14 */ s16 lineX1;
    /* 0x16 */ s16 lineZ1;
    /* 0x18 */ u8 bound;
    /* 0x19 */ u8 active;
} RoomFxMirror;

extern void *g_GeomVramPacketDst;
extern void *g_RoomFxMirrorHandlers[7];
extern GteMatrix g_RoomFxMirrorAxes;

int RoomFx_MirrorNoOp0(void);
int RoomFx_MirrorReset(RoomFxMirror *mirror);
int RoomFx_MirrorSelect(RoomFxMirror *mirror, int cancel, u32 mode, int a, int b);
int RoomFx_MirrorTick(RoomFxMirror *mirror);
int RoomFx_MirrorNoOp4(void);
int RoomFx_MirrorKill(u8 *state);
int RoomFx_MirrorNoOp6(void);
void RoomFx_ModelBind(RoomFxModelGeom *geom, RoomFxModelOwner *owner,
                      int x0, int z0, int x1, int z1);
void RoomFx_ModelUpdate(RoomFxModelGeom *geom);

#endif
