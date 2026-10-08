#ifndef PE1_MENU_BONUS_STATS_H
#define PE1_MENU_BONUS_STATS_H

#include "common.h"

/* Nine-word arrays at 0x800A18B4, 0x800A18D8 and 0x800A18FC.
 * Seven growth categories are normally initialized; the parasite-screen
 * commit loop transfers nine entries, matching AyaSaveState.stats.transfer. */
typedef struct MenuBonusStatWorkspace {
    s32 queryResults[9];
    s32 deltas[9];
    s32 multipliers[9];
} MenuBonusStatWorkspace;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuBonusStatWorkspace, deltas) == 0x24,
                  menu_bonus_stat_deltas_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuBonusStatWorkspace, multipliers) == 0x48,
                  menu_bonus_stat_multipliers_offset);
PE1_STATIC_ASSERT(sizeof(MenuBonusStatWorkspace) == 0x6C,
                  menu_bonus_stat_workspace_size);

extern MenuBonusStatWorkspace D_800A18B4;
extern s32 D_800A18D8[9], D_800A18FC[9];
/* Independent interior labels retain the retail address calculations. */
extern s32 g_BonusPointStatQueryResults[9];
extern s32 g_BonusPointStatDeltas[9];
extern s32 g_BonusPointStatMultipliers[9];

#endif
