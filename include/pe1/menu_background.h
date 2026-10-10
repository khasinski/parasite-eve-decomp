#ifndef PE1_MENU_BACKGROUND_H
#define PE1_MENU_BACKGROUND_H

#include "common.h"
#include "pe1/game_state.h"
#include "pe1/psyq_gpu.h"

extern DRAWENV D_800BCDC8[2];
extern int g_MenuSaveBgFadeState; /* Fade state. */
extern int g_MenuSaveBgFadeHeight; /* Number of 256-pixel blocks. */
extern int g_MenuSaveBgFadeLutLen; /* Fade curve length. */
extern int g_MenuSaveBgFadeStep; /* Signed curve step. */
extern int g_MenuSaveBgFadeIndex; /* Curve position. */
extern int g_MenuSaveBgFadeTint; /* Fade parameter. */
extern u8 D_800A1878[]; /* Fade curve. */

PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, save_background_source) == 0x178, save_background_source_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, save_background_destination) == 0x17C, save_background_destination_offset);
PE1_STATIC_ASSERT(sizeof(DRAWENV) == 0x5C, menu_background_drawenv_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(DRAWENV, r0) == 0x19, menu_background_drawenv_rgb_offset);

void Menu_SaveBgApplyFadeStep(void);


int Menu_SaveBgIsFadeActive(void);
void Menu_SaveBgAdvanceFade(void);

#endif /* PE1_MENU_BACKGROUND_H */
