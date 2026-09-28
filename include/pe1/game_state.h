#ifndef PE1_GAME_STATE_H
#define PE1_GAME_STATE_H

#include "pe1/game_state_types.h"

#ifdef PE1_GAME_STATE_LEGACY_RAW_VIEW
extern Pe1GameState g_GameStateTyped __asm__("g_GameState");
#else
extern Pe1GameState g_GameState;
extern unsigned int g_GameStateFlags;
#endif

#endif
