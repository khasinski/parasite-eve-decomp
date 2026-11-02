#ifndef PE1_SCENE_E20_HOVER_ORB_H
#define PE1_SCENE_E20_HOVER_ORB_H

#include "pe1/asset_find.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/scene_archive.h"
#include "pe1/scene_e20_flare.h"

/* Scene e20 trail controller and its command/particle records.
 * Reserved members are observed offsets, not recovered field meanings. */
typedef struct SceneE20Color { u8 r,g,b,code; } SceneE20Color;
typedef struct SceneE20Matrix { s16 m[3][3],pad; s32 t[3]; } SceneE20Matrix;
typedef struct SceneEffectSlot {
    SceneE20Vec position, target;
    int duration, pending;
} SceneEffectSlot;
typedef struct SceneE20TrailEffect {
    SceneE20Vec position, rotation, endpoint, target, previousPosition;
    SceneE20Vec commandedEndpoint, previousEndpoint, trailHead, trailTail;
    s16 phase, timer, alpha, reserved4E, duration, reserved52;
    struct { SceneE20Vec head, tail; } history[7];
} SceneE20TrailEffect;
typedef struct SceneE20Node { u32 flags; u8 reserved04[12]; s32 progress; } SceneE20Node;
typedef struct SceneE20Actor { u8 reserved[0x4c]; u32 flags; } SceneE20Actor;
typedef struct SceneE20Event { u8 reserved[13]; u8 active; } SceneE20Event;
typedef struct SceneE20Pool { SceneE20Node *node; u8 reserved04[0x264]; SceneE20Vec position; } SceneE20Pool;
typedef struct SceneE20Channel { int reserved[2]; SceneE20Pool *pool; } SceneE20Channel;
extern SceneE20Color g_SceneE20OrbBeamColor, g_SceneE20OrbEdgeColor, g_SceneE20OrbFlashColorOdd, g_SceneE20OrbFlashColorEven;
extern struct BattleEntity *D_8009D254;
extern struct BattleEntity *D_8009CD10;
extern void *g_SceneE20HoverOrbModel;
int SceneE20_HoverOrbController(int mode, SceneE20TrailEffect *effect, SceneEffectSlot *command);
extern SceneEffectSlot g_SceneE20EffectSlots[];
int rand(void);
void FieldFx_DrawGlowLine(void *from, void *to, void *color0, void *color1, int scale0,
                  int scale1, int mode);

extern s32 g_SceneE20HoverOrbHitCooldown;

PE1_STATIC_ASSERT(sizeof(SceneEffectSlot) == 0x18, scene_e20_slot_size);
PE1_STATIC_ASSERT(sizeof(SceneE20TrailEffect) == 0xC4, scene_e20_trail_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE20TrailEffect, history) == 0x54, scene_e20_history_offset);

#endif
