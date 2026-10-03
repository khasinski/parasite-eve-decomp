#ifndef PE1_FIELD_SPIRAL_H
#define PE1_FIELD_SPIRAL_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_actor.h"
#include "pe1/field_model_draw.h"

/* Declarations for the spiral emitter (0x800D6514). It reads the room render
 * Y override as a one-field record, so this header stands apart from
 * field_anim.h, which declares the same word as a plain s16. */
typedef struct FieldSpiralPool {
    u16 id;
    u16 age;
    char *start;
    char *end;
} FieldSpiralPool;

typedef struct FieldSpiralOwner {
    u8 reserved00[8];
    FieldActor *actor;
} FieldSpiralOwner;

typedef struct FieldSpiralRenderY {
    s16 value;
} FieldSpiralRenderY;

extern FieldSpiralPool *D_800F33E0;
extern FieldSpiralOwner *D_800F32D0;
extern FieldSpiralRenderY D_800942EC;
extern u8 *D_800F32D4;

int func_800CE560(char *pool, int size, int count,
                  int (*callback)(int mode, RenderSpiralSprite *state));
void *func_800CE610(char *pool);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);

#endif
