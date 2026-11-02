#ifndef PE1_SCENE_E19_BLAST_H
#define PE1_SCENE_E19_BLAST_H

#include "pe1/akao_send.h"
#include "pe1/asset_find.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"
#include "pe1/scene_archive.h"

/* Scene e19 blast sequence: loads five shell models, then runs a five-state
 * charge/burst timeline at an actor position, spawning rising sparks while
 * the shells, glow rings and flares expand and fade. */
typedef struct SceneE19Blast {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector origin;
    /* 0x18 */ s16 state;
    /* 0x1A */ s16 timer;
} SceneE19Blast;

typedef struct SceneE19BlastSpark {
    /* 0x00 */ u16 x, y, z;
    /* 0x06 */ s16 pad;
    /* 0x08 */ s16 state;
    /* 0x0A */ s16 timer;
} SceneE19BlastSpark;

typedef struct SceneE19BlastActor {
    u32 flags;
} SceneE19BlastActor;

typedef struct SceneE19BlastChannel {
    s32 reserved[2];
    SceneE19BlastActor **pool; /* 0x08 */
} SceneE19BlastChannel;

typedef struct SceneE19BlastPlayer {
    u8 reserved[0x4C];
    s32 flags; /* 0x4C */
} SceneE19BlastPlayer;

typedef struct SceneE19BlastScript {
    u8 reserved[0xD];
    u8 active; /* 0x0D */
} SceneE19BlastScript;

extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern s32 g_SceneE19BlastGlow;
extern void *g_SceneE19BlastModel0, *g_SceneE19BlastModel1, *g_SceneE19BlastModel2, *g_SceneE19BlastModel3, *g_SceneE19BlastModel4;

int SceneE19_Blast(int mode, SceneE19Blast *blast);
int rand(void);
void FieldFx_ModelSetTexture(int page, int clut);
void Geo_ClipAlternatingArcPoint(int mode, int count, int value);

#endif
