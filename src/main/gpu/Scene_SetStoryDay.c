#include "common.h"
#include "pe1/game_state.h"

extern u32 D_8009D2E8;
/* Existing byte symbols for the shared state's pending/current day and flags.
 * Preserve the unsigned pending-byte read before its signed-day conversion. */
extern volatile u8 D_800B0CE5;
extern u8 D_800B0CE4, D_800B0CE6;

int Scene_SetStoryDay(s32 storyDay) {
    Pe1GameState *gameState = &g_GameState;
    u8 flags;

    if (storyDay == -1) {
        u8 pending;
        u8 storyFlags;
        pending = D_800B0CE5;
        storyFlags = D_800B0CE6;
        storyDay = (s8)pending;
        D_800B0CE4 = pending;
        D_800B0CE6 = storyFlags | 3;
    }

    if (((g_GameStateFlags & 2) != 0) || ((gameState->flags & 2) != 0)) {
        D_800B0CE6 |= 2;
        D_8009D2E8 &= ~2U;
    }

    flags = gameState->story_day_flags;
    if ((flags & 4) != 0) {
        gameState->story_day_flags = (flags | 3) & ~4;
    }

    if ((storyDay - 1U) < 8U) {
        if (storyDay != (s8)gameState->pending_story_day) {
            s8 newStoryDay;

            newStoryDay = storyDay;
            gameState->pending_story_day = newStoryDay;
            gameState->current_story_day = newStoryDay;
            gameState->story_day_flags |= 1;
        }
    }

    return 0;
}
