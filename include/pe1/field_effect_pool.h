#ifndef PE1_FIELD_EFFECT_POOL_H
#define PE1_FIELD_EFFECT_POOL_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_actor.h"
#include "pe1/field_model_draw.h"

/* Effect pool declarations for field engine emitters that read the room
 * render Y override as a one-field record, so its load stays below the
 * in-struct position stores like retail. field_anim.h declares the same word
 * as a plain s16 for its other users, so these units include this header
 * instead of field_anim.h. */
typedef struct FieldEffectPool {
    u16 id;
    u16 age;
    char *start;
    char *end;
} FieldEffectPool;

typedef struct FieldEffectOwner {
    u8 reserved00[8];
    FieldActor *actor;
} FieldEffectOwner;

typedef struct FieldEffectRenderY {
    s16 value;
} FieldEffectRenderY;

typedef int (*FieldEffectCallback)(int mode, void *state);

extern FieldEffectPool *D_800F33E0;
extern FieldEffectOwner *D_800F32D0;
extern FieldEffectRenderY D_800942EC;
extern u8 *D_800F32D4;

int func_800CE560(char *pool, int size, int count, FieldEffectCallback callback);
void *func_800CE610(char *pool);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);

#endif
