/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

s32 MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_DestroyNode();
void Menu_StepSaveLoadScreen(void);
void Menu_CreateLevelUpResultPanel(void);
void Inv_SetActiveList(s32 arg0, s32 arg1);
void Menu_PlayConfirmSound(void);
void Akao_SetBgmVolumeFade(void);
void Akao_FlushBgmVolumeFade(void);
s32 Menu_GetBattleCount(void);
s32 Stat_BinarySearch(s32 value, void *table);
void *Aya_GetLevelExpTable(void);

extern s32 g_MenuPendingTotalExp;
extern s32 g_MenuExpAllocTarget;
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])

s32 Menu_ConfirmExpAllocation(s32 unused, s32 flags) {
    s32 confirm;
    s32 new_level;

    confirm = flags & 0x10000;
    if (confirm != 0) {
        if (g_MenuPendingTotalExp < g_MenuExpAllocTarget) {
            g_MenuPendingTotalExp = g_MenuExpAllocTarget;
        } else {
            MenuWidget_DestroyNode();
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
            Akao_FlushBgmVolumeFade();
            new_level = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
            if (new_level != Stat_BinarySearch(g_AyaSaveTotalExp, Aya_GetLevelExpTable())) {
                Menu_CreateLevelUpResultPanel();
                Akao_SetBgmVolumeFade();
            } else if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
            g_AyaSaveTotalExp = g_MenuPendingTotalExp;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}
