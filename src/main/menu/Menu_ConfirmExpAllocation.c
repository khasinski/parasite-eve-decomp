/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

s32 MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_DestroyNode() __asm__("func_80062F1C");
void Menu_StepSaveLoadScreen(void);
void Menu_CreateLevelUpResultPanel(void) __asm__("func_8004BE4C");
void Inv_SetActiveList(s32 arg0, s32 arg1);
void Menu_PlayConfirmSound(void) __asm__("func_800525EC");
void Akao_SetBgmVolumeFade(void);
void Akao_FlushBgmVolumeFade(void);
s32 Menu_GetBattleCount(void);
s32 Stat_BinarySearch(s32 value, void *table);
void *Aya_GetLevelExpTable(void) __asm__("func_8005DBF8");

extern s32 g_MenuPendingTotalExp;
extern s32 g_MenuExpAllocTarget;
extern struct { char _[16]; } g_AyaSaveTotalExp_o __asm__("g_AyaSaveTotalExp");
#define g_AyaSaveTotalExp (*(s32 *)&g_AyaSaveTotalExp_o)

asm(".globl func_8004BB80");
asm("func_8004BB80 = Menu_ConfirmExpAllocation");

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
