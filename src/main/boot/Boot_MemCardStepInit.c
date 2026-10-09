#include "common.h"
#include "pe1/memcard.h"
#include "pe1/task_node.h"
#include "pe1/game_state_types.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Battle_StartEncounter();
M2C_UNK func_80042F20();
s32 Asset_LoadTimTextures();
s32 CD_StepReadState();
extern s32 g_SceneDataTable0;
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
extern s32 D_8009D1A0_rd[] __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern TaskNode *g_TaskNodePool;
extern Pe1GameState g_GameState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, memcard_init_phase) == 0xF4,
                  memcard_init_phase_offset);

s32 Boot_MemCardStepInit(u8 **arg0) {
    Pe1GameState *base;

    base = &g_GameState;
    loop_1:
    switch (base->memcard_init_phase) {
    case 0x0:
        if (!(base->story_day_flags & 3)) {
            base->memcard_init_phase = 0x37U;
        }
        base->flags = base->flags | 0x800000;
        goto block_19;
    case 0x37:
        if (!(g_GameState.flags & 0x400000)) {
            MemCard_InitSlotState();
        }
        base->memcard_init_phase = 0x38U;
        goto loop_1;
    case 0x38:
        if (CD_StepReadState(1) != 1) {
            if (!(g_GameState.flags & 0x400000)) {
                func_80042F20();
            }
            base->memcard_init_phase = 0x39U;
            goto loop_1;
        }
        goto block_19;
    case 0x39:
        if (Asset_LoadTimTextures(1) != 1) {
            base->memcard_init_phase = 0x3AU;
            goto loop_1;
        }
        goto block_19;
    case 0x3A:
        {
            s32 t1a0 = D_8009D1A0_rd[0] | 2;
            g_GameStateFlags = t1a0;
        }
        Battle_StartEncounter(**arg0);
        base->memcard_init_phase = 0x3BU;
        goto block_19;
    case 0x3B:
        if (!(base->story_day_flags & 3)) {
            base->memcard_init_phase = 0U;
            base->flags = base->flags & 0xFF7FFFFF;
            __asm__ volatile("");
            return 1;
        }
        break;
    default:
        return 1;
    }
block_19:
    g_SceneDataTable0 -= 0xC;
    g_TaskNodePool->active = 1;
    return 0;
}
