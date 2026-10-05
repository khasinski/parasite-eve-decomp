/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_palette.h"
#include "pe1/battle.h"
#include "pe1/battle_start.h"


void Battle_ResetEnemyStats(int mode) {
    register Combatant *actor asm("$5");
    Combatant *clear_actor;
    register s32 flags asm("$2");

    actor = D_8009D278;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    D_8009D278->actionMode12 = 4;
    actor = D_8009D278;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    D_8009D1D0 = 0;

    if ((mode & 0xFF) == 1) {
        D_8009D234[0] = 0x5A;
        D_8009D244 = 1;
        asm volatile("" ::: "memory");
        flags = actor->stateFlags;
        flags |= 0x800000;
    } else {
        flags = actor->stateFlags;
        D_8009D244 = 0;
        flags &= 0xFFBFFFFF;
        flags &= 0xFF7FFFFF;
    }
    actor->stateFlags = flags;
    asm volatile("" ::: "memory");

    clear_actor = D_8009D278;
    flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    flags &= -4;
    flags &= -0xD;
    flags &= -0x31;
    flags &= -0xC1;
    flags &= -0x101;
    flags &= -0x201;
    flags &= -0x401;
    flags &= -0x801;
    flags &= -0x1001;
    flags &= -0x2001;
    flags &= -0x4001;
    flags &= 0xFFFF7FFF;
    flags &= 0xFFF7FFFF;
    flags &= 0xFFFEFFFF;
    flags &= 0xFFF9FFFF;
    flags &= 0xFFEFFFFF;
    flags &= 0xFFDFFFFF;
    flags &= 0xFEFFFFFF;
    flags &= 0xF1FFFFFF;
    flags &= 0xEFFFFFFF;
    flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = flags;

    D_8009D278->panelB_timer = 0;
    D_8009D278->panelAux_timer = 0;
    {
        u32 state;
        state = reset_flags_read[0];
        reset_flags_write[0] = state & ~0x10;
    }
    Battle_FlushScriptSounds();
    Tbl_ResetAll();
    D_8009D298[0] = 0;
    D_8009D29A[0] = 0;
    D_8009D29B[0] = 0;
    D_8009D29C[0] = 0;
}


#define PALETTE_BYTE(name) (*(s8 *)&(name))
#define PALETTE_WORD(name) (*(u32 *)&(name))
#define PALETTE_POINTER(name) (*(BattleEntity **)&(name))

void Battle_SetupPlayerPalette(void) {
    int c2;
    int c3;
    int c4;
    int c5;
    int c6;
    register int c7 asm("$7");
    u8 *flags;

    c6 = 0x82;
    c5 = 0x36;
    c4 = 0x4A;
    c2 = 0xFF;
    asm("" : : "r"(c2));
    c3 = 0x3B;
    c7 = 0x3D;
    asm("" : : "r"(c2), "r"(c7));

    PALETTE_BYTE(D_800B0135) = c6;
    PALETTE_BYTE(D_800B0145) = c6;
    PALETTE_BYTE(D_800B017D) = c6;
    PALETTE_BYTE(D_800B018D) = c6;

    c6 = 0x81;
    PALETTE_BYTE(D_800B0136) = c5;
    PALETTE_BYTE(D_800B0146) = c5;
    PALETTE_BYTE(D_800B017E) = c5;
    PALETTE_BYTE(D_800B018E) = c5;

    c5 = 0x83;
    PALETTE_BYTE(D_800B013C) = c4;
    PALETTE_BYTE(D_800B014C) = c4;
    PALETTE_BYTE(D_800B0184) = c4;
    PALETTE_BYTE(D_800B0194) = c4;

    c4 = 0x13;
    PALETTE_BYTE(D_800B013E) = c3;
    PALETTE_BYTE(D_800B014E) = c3;
    PALETTE_BYTE(D_800B0186) = c3;
    PALETTE_BYTE(D_800B0196) = c3;

    c3 = 1;
    PALETTE_BYTE(D_800B0134) = 0;
    PALETTE_BYTE(D_800B013D) = c2;
    PALETTE_BYTE(D_800B0144) = 0;
    PALETTE_BYTE(D_800B014D) = c2;
    PALETTE_BYTE(D_800B017C) = 0;
    PALETTE_BYTE(D_800B0185) = c2;
    PALETTE_BYTE(D_800B018C) = 0;
    PALETTE_BYTE(D_800B0195) = c2;

    PALETTE_BYTE(D_800B0158) = c2;
    PALETTE_BYTE(D_800B0159) = c7;
    PALETTE_BYTE(D_800B015A) = c6;
    PALETTE_BYTE(D_800B0160) = c5;
    PALETTE_BYTE(D_800B0161) = c4;
    PALETTE_BYTE(D_800B0162) = c3;
    PALETTE_BYTE(D_800B0168) = c2;
    PALETTE_BYTE(D_800B0169) = c7;
    PALETTE_BYTE(D_800B016A) = c6;
    PALETTE_BYTE(D_800B0170) = c5;
    PALETTE_BYTE(D_800B0171) = c4;
    PALETTE_BYTE(D_800B0172) = c3;
    PALETTE_BYTE(D_800B01A0) = c2;
    PALETTE_BYTE(D_800B01A1) = c7;
    PALETTE_BYTE(D_800B01A2) = c6;
    PALETTE_BYTE(D_800B01A8) = c5;
    PALETTE_BYTE(D_800B01A9) = c4;
    PALETTE_BYTE(D_800B01AA) = c3;
    PALETTE_BYTE(D_800B01B0) = c2;
    PALETTE_BYTE(D_800B01B1) = c7;
    PALETTE_BYTE(D_800B01B2) = c6;
    PALETTE_BYTE(D_800B01B8) = c5;
    PALETTE_BYTE(D_800B01B9) = c4;
    PALETTE_BYTE(D_800B01BA) = c3;

    Battle_FlushScriptSounds();
    D_8009D2EC = 0;
    D_8009D2A0 = 0;
    PALETTE_POINTER(D_8009D254_palette_view)->actionCheckFn =
        (void *)PALETTE_WORD(D_800915E0);
    *(u32 *)&D_8009D1A0_write = PALETTE_WORD(D_8009D1A0_read) & -3;
    *(u32 *)&D_8009D2E8_write = PALETTE_WORD(D_8009D2E8_read) & -0x11;
    flags = &PALETTE_BYTE(D_800B0CE6);
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}


void Battle_StartEncounter(int mode) {
    register int i asm("$4");
    register int neg_one asm("$5");
    BattleEntity *entity;
    Combatant *actor;
    u32 value;
    int mode_reg;
    entity = D_8009D254[0];
    D_8009D278 = (Combatant *)entity->core;
    D_8009D1E8 = 0;
    (*(u32 *)&battle_start_phase_storage) = 0;
    D_8009D28C = 0;
    D_8009CE7C = 0;
    D_8009CE78 = 0;
    D_8009D288 = 0;
    D_8009CE74 = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    battle_start_flags_byte[0] = 0;
    asm volatile("" : : : "memory");
    { register u32 initial_flags asm("$3");
    initial_flags = battle_start_flags_read[0];
    D_8009D1A8[0] = 0;
    D_8009D1CE[0] = 0;
    D_8009D235[0] = 0;
    D_8009D304 = 0;
    D_8009D21C = 0;
    battle_start_flags_write[0] = initial_flags & -0x301;
    }

    for (; (u8)i < 0xA; i++) {
        D_800A7FF0[(u8)i].id = 0;
        D_800A7FF0[(u8)i].extra = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        D_800B8A90[(u8)i] = 0;
    }

    srand((*(u32 *)&battle_start_seed_storage));
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = D_8009D278;
    value = actor->exp_or_acc;
    if ((s32)value <= 0) {
        actor->exp_or_acc = 0x10000;
    } else {
        u32 max_value;

        max_value = actor->maxAtk;
        if ((s32)value >= (s32)max_value) {
            actor->exp_or_acc = max_value;
            actor->atbGauge = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        BattleEntity *post_entity;
        RenderMatrix *post_actor;
        u32 callback;
        post_entity = D_8009D254[0];
        post_actor = post_entity->renderObject.matrices;
        callback = (u32)Entity_CheckActionIdMatch;
        post_entity->actionCheckFn = (void *)callback;
        (*(s16 *)&battle_start_camera_storage) = post_actor->translation[1] - 0x64;
    }
    Window_SetBoundsByMode(mode_reg & 0xFF);
    {
        Combatant *final_actor;
        void *final_entity;
        final_actor = D_8009D278;
        final_entity = battle_start_final_entity[0];
        Entity_SetActionMode(final_entity, final_actor->actionMode12);
    }
}
