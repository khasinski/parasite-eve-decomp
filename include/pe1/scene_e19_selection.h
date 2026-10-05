#ifndef PE1_SCENE_E19_SELECTION_H
#define PE1_SCENE_E19_SELECTION_H

#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/field_actor_script_state.h"
#include "pe1/render_object.h"

/* Scene e19 grab sequence: keeps the player locked to the selection matrix
 * while the partner actor plays its animation and fires events on frame
 * crossings. */

/* View of the partner actor's script state: the byte record at 0x18 is the
 * substate the scene flips between 1, 2 and 4. */
typedef FieldActorScriptState SceneE19ActorState;

typedef struct SceneE19SelectionTail {
    /* 0x00 */ u8 pad00[4];
    /* 0x04 */ int *signal;
    /* 0x08 */ u8 pad08[8];
} SceneE19SelectionTail;

typedef struct SceneE19Selection {
    /* 0x00 */ u8 pad00[8];
    /* 0x08 */ FieldActor *actor;
    /* 0x0C */ SceneE19SelectionTail tail;
    /* 0x1C */ RenderMatrix matrix;
    /* 0x3C */ int savedX;
    /* 0x40 */ int savedZ;
} SceneE19Selection;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Selection, matrix) == 0x1C,
                  scene_e19_selection_matrix_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Selection, savedZ) == 0x40,
                  scene_e19_selection_saved_z_offset);

extern FieldActor *g_PlayerEntity;
void func_80020CE4(void);
void func_80192B10(FieldActor *actor, SceneE19SelectionTail *tail);
int func_80192BFC(SceneE19Selection *selection);

#endif
