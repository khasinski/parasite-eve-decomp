#ifndef PE1_SAVE_TAIL_STATE_H
#define PE1_SAVE_TAIL_STATE_H

#include "common.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_saved_state.h"
#include "pe1/entity_allocation.h"
#include "pe1/game_state.h"
#include "pe1/save_blob.h"
#include "pe1/scene_dispatch.h"

/* Save tail cursor: Save_SerializeTail/Save_DeserializeTail copy each
 * record below through it and advance it by the record size. */
extern u8 *g_SaveIoCursor;

/* Records the save tail copies as raw bytes besides the game state. */
extern u32 D_8009D2E8;
extern unsigned int D_8009CDA8;           /* field movement lock flags */
/* g_ScreenTint.fadeMode as its own byte symbol. Draw_InitPipeline (a -G8
 * unit) sets its bit 6 through it: the scroll-state view would be placed
 * in small data there. */
extern u8 g_ScreenTransitionState[];

#endif
