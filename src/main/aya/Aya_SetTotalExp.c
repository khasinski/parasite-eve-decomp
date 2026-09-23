/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/aya.h"
#include "pe1/psyq_nop.h"

void Menu_SetBattleEquipMode(s32 mode);
void BattleCmd_SyncActiveAmmo(void);
void *Aya_GetLevelExpTable(void);
s32 Stat_BinarySearch(s32 value, void *table);

s32 Battle_GetScaledMaxHP(s32 level);
void Inv_LoadWayneItemsAsOverride(void *items);
void Menu_CreateExpReviewView(void);
void Akao_SetBgmVolumeFade(void);
void Menu_ResetInputState(void);



/* Distinct views preserve the original loads and store around the bonus update. */
extern u16 D1E_read[16] __asm__("D_800C0E1E");
extern u16 D1E_write[16] __asm__("D_800C0E1E");
extern u16 D1E_bonus[16] __asm__("D_800C0E1E");
/* This loop sign-extends the stat allocations (lh in the original). */
extern s16 D_800C0E28_signed[7] __asm__("D_800C0E28");
extern struct { char bytes[32]; } stat_out_alias __asm__("D_800A18B4");
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

extern AyaSaveState D_800C0E00_array[] __asm__("D_800C0E00");
extern u8 aya_level_view[16] __asm__("D_800C0E0A");
extern u16 aya_max_hp_view[16] __asm__("D_800C0E06");
extern u32 aya_bonus_view[4] __asm__("D_800C0E10");

void Aya_SetTotalExp(s32 exp_delta, s32 pe_bonus_delta, void *wayne_items) {
    s32 exp_delta_reg;
    s32 pe_bonus_delta_reg;
    void *wayne_items_reg;
    register AyaSaveState *save asm("$18");
    s32 previous_exp;
    register s16 *stat_src asm("$19");
    s32 i;
    s32 stat_level;

    exp_delta_reg = exp_delta;
    pe_bonus_delta_reg = pe_bonus_delta;
    wayne_items_reg = wayne_items;

    Menu_SetBattleEquipMode(0);
    BattleCmd_SyncActiveAmmo();

    save = &D_800C0E00_array[0];
    asm volatile("" : "=r"(save) : "0"(save));
    previous_exp = save->total_exp;
    PE1_NOP();
    D_8009CFE8 = previous_exp;
    asm volatile("" : : : "memory");
    D_8009CFEC = previous_exp + exp_delta_reg;
    D_8009CFF0 = Stat_BinarySearch(D_8009CFE8, Aya_GetLevelExpTable());
    D_8009CEFC = 1;

    D1E_write[0] = D1E_read[0] + pe_bonus_delta_reg;
    D_8009CF60 = aya_level_view[0];
    D_8009CF64 = aya_max_hp_view[0];
    D_8009CF74 = aya_bonus_view[0];
    D_8009CF68 = aya_bonus_view[0];
    D_8009CF6C = Stat_BinarySearch(D_8009CFEC, Aya_GetLevelExpTable());

    if (D_8009CF60 < D_8009CF6C) {
        register s32 cap asm("$2") = 0x1869F;
        register s32 result asm("$5") = 0x1869F;
        s32 bonus = D_8009CF74 + D1E_bonus[0];
        if (bonus <= cap) result = bonus;
        D_8009CF74 = result;
        save->pe_bonus_pool = 0;
    }

    stat_src = D_800C0E28_signed;
    i = 0;
    do {
        D_800A18D8[i] = *stat_src++;
        Stat_QueryLevelAndSubLevel(i, D_800A18D8[i], (s32 *)((u8 *)&stat_out_alias + i * 4), 0);
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
