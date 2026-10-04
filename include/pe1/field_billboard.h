#ifndef PE1_FIELD_BILLBOARD_H
#define PE1_FIELD_BILLBOARD_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_tint.h"
#include "pe1/field_textured_strip.h"

/* Field engine billboard (func_800C3B04): a textured quad centred on a
 * projected point, sized by the cell size, a 20.12 scale and the
 * perspective distance, optionally turned about the view axis. */

typedef union FieldBillboardScreen {
    s32 word;
    struct {
        s16 x;
        s16 y;
    } xy;
} FieldBillboardScreen;

/* Scratchpad work area published through D_800E284C. */
typedef struct FieldBillboardScratch {
    /* 0x00 */ s32 p;
    /* 0x04 */ s32 flag;
    /* 0x08 */ FieldBillboardScreen screen;
    /* 0x0C */ s32 depth;
    /* 0x10 */ u8 u;
    /* 0x11 */ u8 v;
    /* 0x12 */ u8 clutX;
    /* 0x13 */ u8 clutY;
    /* 0x14 */ u8 pad14[4];
    /* 0x18 */ u32 channel;
    /* 0x1C */ s16 centerX;
    /* 0x1E */ s16 centerY;
    /* 0x20 */ u8 pad20[0x20];
    /* 0x40 */ GteShortVector corner[4];
    /* 0x60 */ GteShortVector turned[4];
    /* 0x80 */ u8 rgb[4];
    /* 0x84 */ GteMatrix rotation;
} FieldBillboardScratch;

#define FIELD_BILLBOARD_SCRATCH ((FieldBillboardScratch *)0x1F800000)

extern FieldBillboardScratch *D_800E284C;

typedef struct FieldBillboard {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ u8 pad08[4];
    /* 0x0C */ s16 angle;
    /* 0x0E */ u8 pad0E[2];
    /* 0x10 */ s32 scaleX;
    /* 0x14 */ s32 scaleY;
    /* 0x18 */ u8 pad18[8];
    /* 0x20 */ u8 rgb[4];
    /* 0x24 */ u8 cell;
    /* 0x25 */ u8 clut;
    /* 0x26 */ s16 depth;
    /* 0x28 */ u16 brightness;
} FieldBillboard;

/* Points at the projection distance (H) of the current view; read as a
 * one-field record so the load stays below the scratch stores. */
typedef struct FieldProjectionSlot {
    s32 *distance;
} FieldProjectionSlot;

extern FieldProjectionSlot D_800BCFA8;

int RotTransPers(GteShortVector *vector, s32 *sxy, s32 *p, s32 *flag);

#endif
