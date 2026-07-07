/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
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
                temp_a2 = M2C_FIELD(var_s0, void **, 0);
                if (temp_a2 != NULL) {
                    temp_a0 = M2C_FIELD(var_s0, s32 *, 0x98);
                    if (((temp_a0 & 0x2040) != 0x40) && !(temp_a0 & 0x4000) && (M2C_FIELD(temp_a2, s32 *, 0x10) > 0)) {
                        off1 = g_BattleTargetIndex * 0xC;
                        E000_AT(off1) = var_s0;
                        res = Battle_CalcDistToPlayer(var_s0);
                        off = g_BattleTargetIndex * 0xC;
                        E004_AT(off) = res;
                        res2 = Gte_Atan2(
                            M2C_FIELD(var_s0, s16 *, 0x268) - M2C_FIELD(g_PlayerEntity_2, s16 *, 0x2A),
                            M2C_FIELD(var_s0, s16 *, 0x26C) - M2C_FIELD(g_PlayerEntity_2, s16 *, 0x32));
                        tmpc = g_BattleTargetIndex + 1;
                        off = g_BattleTargetIndex * 0xC;
                        E008_AT(off) = res2;
                        g_BattleTargetIndex = tmpc;
                    }
                }
            }
            var_s0 = M2C_FIELD(var_s0, void **, 4);
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
