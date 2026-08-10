#include "common.h"
#include "pe1/battle.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 Battle_CalcDistToPlayer();
s32 Gte_Atan2();
M2C_UNK Battle_SortTargets();
extern s8 g_BattleTargetIndex;
extern struct { char _[16]; } D_8009D20C_o __asm__("g_FieldActorListHead");
#define g_FieldActorListHead (*(void **)&D_8009D20C_o)
extern struct { char _[16]; } g_PlayerEntity_o __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_o2 __asm__("g_PlayerEntity");
extern struct { char _[16]; } g_PlayerEntity_o3 __asm__("g_PlayerEntity");
#define g_PlayerEntity (*(void **)&g_PlayerEntity_o)
#define g_PlayerEntity_2 (*(void **)&g_PlayerEntity_o2)
#define g_PlayerEntity_3 (*(void **)&g_PlayerEntity_o3)
extern struct { char _[16]; } D_8009E000_o __asm__("g_BattleTargetList");
extern struct { char _[16]; } D_8009E004_o __asm__("g_BattleTargetDistance");
extern struct { char _[16]; } D_8009E008_o __asm__("D_8009E008");
#define E000_AT(off) (*(void **)((u8 *)&D_8009E000_o + (off)))
#define E004_AT(off) (*(s32 *)((u8 *)&D_8009E004_o + (off)))
#define E008_AT(off) (*(s16 *)((u8 *)&D_8009E008_o + (off)))
#define ENTITY_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(BattleEntity, member)))
#define ENEMY_FIELD(base, type, member) \
    (*(type)((char *)(base) + PE1_OFFSETOF(EnemyCombatant, member)))

void Battle_BuildTargetList(void) {
    s32 temp_a0;
    s32 off;
    s32 off1;
    register s32 tmpc asm("$5");
    register s32 cfin asm("$2");
    s32 pad_[2];
    s32 res;
    s16 res2;
    register void *temp_a2 asm("$6");
    register void *dp asm("$5");
    void *var_s0;

    var_s0 = g_FieldActorListHead;
    g_BattleTargetIndex = 0;
    if (var_s0 != NULL) {
        do {
            dp = g_PlayerEntity;
            if (var_s0 != dp) {
                temp_a2 = ENTITY_FIELD(var_s0, void **, core);
                if (temp_a2 != NULL) {
                    temp_a0 = ENTITY_FIELD(var_s0, s32 *, entityFlags);
                    if (((temp_a0 & 0x2040) != 0x40) && !(temp_a0 & 0x4000) &&
                        (ENEMY_FIELD(temp_a2, s32 *, hpAlive) > 0)) {
                        off1 = g_BattleTargetIndex * 0xC;
                        E000_AT(off1) = var_s0;
                        res = Battle_CalcDistToPlayer(var_s0);
                        off = g_BattleTargetIndex * 0xC;
                        E004_AT(off) = res;
                        res2 = Gte_Atan2(
                            ENTITY_FIELD(var_s0, s16 *, targetX) -
                                ENTITY_FIELD(g_PlayerEntity_2, s16 *, posX.parts.integer),
                            ENTITY_FIELD(var_s0, s16 *, targetZ) -
                                ENTITY_FIELD(g_PlayerEntity_2, s16 *, posZ.parts.integer));
                        tmpc = g_BattleTargetIndex + 1;
                        off = g_BattleTargetIndex * 0xC;
                        E008_AT(off) = res2;
                        g_BattleTargetIndex = tmpc;
                    }
                }
            }
            var_s0 = ENTITY_FIELD(var_s0, void **, next);
        } while (var_s0 != NULL);
    }
    if (g_BattleTargetIndex >= 2) {
        Battle_SortTargets(&D_8009E000_o, 0, (s8) (g_BattleTargetIndex - 1));
    }
    cfin = g_BattleTargetIndex;
    __asm__("" : "=r"(cfin) : "0"(cfin));
    off = cfin * 0xC;
    E000_AT(off) = NULL;
}

#undef ENEMY_FIELD
#undef ENTITY_FIELD
