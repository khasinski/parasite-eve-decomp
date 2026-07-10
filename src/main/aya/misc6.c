/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


#include "include_asm.h"
#include "pe1/aya.h"

extern s32 D_8009CFE4;

void Draw_SetBaseOffsetPosition(s32 x, s32 y);
s32 Draw_GetBaseY(void);
void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void MenuWidget_DestroyNode(void *node);
void Menu_SetBattleEquipMode(s32 mode);
void BattleCmd_SyncActiveAmmo(void);
void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);
void Stat_QueryLevelAndSubLevel(s32 category, s32 value, s32 *out_level, s32 flag);
s32 Battle_GetScaledMaxHP(s32 level);
void Inv_LoadWayneItemsAsOverride(void *items);
void Menu_CreateExpReviewView(void);
void Akao_SetBgmVolumeFade(void);
void Menu_ResetInputState(void);

extern AyaSaveState D_800C0E00[];
#define D_800C0E00 (D_800C0E00[0])
extern u16 D_800C0E1E[];
#define D_800C0E1E (D_800C0E1E[0])
extern u16 D_800C0E28[];
extern s32 D_800A18B4[];
extern s32 D_800A18D8[];
extern s32 D_800A18FC[];
extern s32 D_8009CFE8;
extern s32 D_8009CFEC;
extern s32 D_8009CFF0;
extern s32 D_8009CEFC;
extern s32 D_8009CF60;
extern s32 D_8009CF64;
extern s32 D_8009CF68;
extern s32 D_8009CF6C;
extern s32 D_8009CF70;
extern s32 D_8009CF74;
extern s32 D_8009CF84;

s32 Menu_StepScrollList(void *node, s32 input)
{
    s32 zero_arg;

    if (input & 0x1000) {
        zero_arg = 0;
        input = -1;
        goto move;
    }

    if (input & 0x4000) {
        zero_arg = 0;
        input = 1;
move:
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }

    if (input & 0x10000) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if (input & 0x40) {
        Draw_SetBaseOffsetPosition(0, D_8009CFE4 - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}

void Aya_SetTotalExp(s32 exp_delta, s32 pe_bonus_delta, void *wayne_items) {
    register s32 exp_delta_reg asm("$16");
    register s32 pe_bonus_delta_reg asm("$17");
    register void *wayne_items_reg asm("$20");
    register AyaSaveState *save asm("$18");
    register u16 *stat_src asm("$19");
    s32 i;
    s32 stat_level;

    exp_delta_reg = exp_delta;
    pe_bonus_delta_reg = pe_bonus_delta;
    wayne_items_reg = wayne_items;

    Menu_SetBattleEquipMode(0);
    BattleCmd_SyncActiveAmmo();

    save = &D_800C0E00;
    D_8009CFE8 = save->total_exp;
    D_8009CFEC = save->total_exp + exp_delta_reg;
    D_8009CFF0 = Stat_BinarySearch(D_8009CFE8, Aya_GetLevelExpTable());
    D_8009CEFC = 1;

    D_800C0E1E += pe_bonus_delta_reg;
    D_8009CF60 = save->level;
    D_8009CF64 = save->max_hp;
    D_8009CF74 = save->bonus_points;
    D_8009CF68 = save->bonus_points;
    D_8009CF6C = Stat_BinarySearch(D_8009CFEC, Aya_GetLevelExpTable());

    if (D_8009CF60 < D_8009CF6C) {
        s32 bonus;

        bonus = D_8009CF74 + D_800C0E1E;
        if (bonus > 0x1869F) {
            bonus = 0x1869F;
        }
        D_8009CF74 = bonus;
        save->pe_bonus_pool = 0;
    }

    stat_src = D_800C0E28;
    i = 0;
    do {
        D_800A18D8[i] = *stat_src++;
        Stat_QueryLevelAndSubLevel(i, D_800A18D8[i], D_800A18B4 + i, 0);
        D_800A18FC[i] = (D_8009CF6C - D_8009CF60) * 10;
        i++;
    } while (i < 7);

    Stat_QueryLevelAndSubLevel(0, D_800A18D8[0] + D_800A18FC[0], &stat_level, 0);
    D_8009CF70 = Battle_GetScaledMaxHP(stat_level);
    D_8009CF84 = 2;
    Inv_LoadWayneItemsAsOverride(wayne_items_reg);
    Menu_CreateExpReviewView();
    Akao_SetBgmVolumeFade();
    Menu_ResetInputState();
}
