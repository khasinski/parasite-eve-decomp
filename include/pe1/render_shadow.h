#ifndef PE1_RENDER_SHADOW_H
#define PE1_RENDER_SHADOW_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/render_tint.h"

/* Ground shadow quad drawn under a field actor (POLY_FT4 layout). */
typedef struct RenderShadowQuad {
    RenderGpuTag tag;
    u8 r0, g0, b0, code;
    s32 xy0; u8 u0, v0; u16 clut;
    s32 xy1; u8 u1, v1; u16 tpage;
    s32 xy2; u8 u2, v2; u16 pad2;
    s32 xy3; u8 u3, v3; u16 pad3;
} RenderShadowQuad;

/* 16.16 world coordinate; the shadow uses the integer half. */
typedef struct RenderShadowCoord {
    int fraction : 16;
    int integer : 16;
} RenderShadowCoord;

/* Field actor view used by the shadow pass: the actor record continues past
 * the common FieldActor prefix with the shadow packets and parameters. */
typedef struct RenderShadowActor {
    /* 0x000 */ u8 pad000[0x28];
    /* 0x028 */ RenderShadowCoord x, y, z;
    /* 0x034 */ u8 pad034[0x64];
    /* 0x098 */ u32 flags;
    /* 0x09C */ u8 pad09C[0x118];
    /* 0x1B4 */ RenderObjectHeader *header;
    /* 0x1B8 */ u8 pad1B8[0x80];
    /* 0x238 */ GteMatrixStorage *matrices;
    /* 0x23C */ u8 pad23C[0xC];
    /* 0x248 */ u8 fade_red, fade_green, fade_blue;
    /* 0x24B */ u8 pad24B[5];
    /* 0x250 */ u16 render_flags;
    /* 0x252 */ u8 visible;
    /* 0x253 */ u8 pad253[0x25];
    /* 0x278 */ RenderShadowQuad *shadow_quads; /* one per draw buffer */
    /* 0x27C */ u8 shadow_matrix_index;
    /* 0x27D */ u8 shadow_intensity;
} RenderShadowActor;

PE1_STATIC_ASSERT(sizeof(RenderShadowQuad) == 40, render_shadow_quad_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderShadowActor, flags) == 0x98,
                  render_shadow_actor_flags);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderShadowActor, matrices) == 0x238,
                  render_shadow_actor_matrices);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderShadowActor, render_flags) == 0x250,
                  render_shadow_actor_render_flags);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RenderShadowActor, shadow_quads) == 0x278,
                  render_shadow_actor_quads);

void SetRotMatrix(GteMatrix *matrix);
void SetTransMatrix(GteMatrix *matrix);
int RotTransPers(GteShortVector *vector, s32 *sxy, s32 *p, s32 *flag);
int Render_DrawRoom(RenderShadowActor *actor);

#endif
