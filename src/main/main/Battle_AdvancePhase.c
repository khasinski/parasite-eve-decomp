/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: --expand-div -G8 */

#include "common.h"
#include "pe1/battle.h"
#include <m2c_macros.h>

#define NULL ((void *)0)
typedef struct { u8 bytes[0x14]; } __attribute__((packed)) Tbl20;
typedef struct { u8 bytes[0x28]; } __attribute__((packed)) Tbl40;
typedef struct { s32 words[4]; } Copy16;
typedef struct { u8 bytes[0x10]; } __attribute__((packed)) PackedCopy16;
typedef struct { u8 bytes[8]; } __attribute__((packed)) PackedCopy8;
typedef struct { u8 bytes[0x10]; } LargeSymbol;
typedef struct { s32 entity; s16 command; u8 pad[2]; } QueueEntry;
typedef struct { u16 value; u8 pad[6]; } QueueCommand;
typedef union { s32 value; } FlagValue;

void Akao_SendPositionalCmdStereo();
void BattleCmd_CommitAmmoAndUpdate();
void Battle_ApplySpellEffect();
s16 Battle_CalcAngleToTarget();
s32 Battle_CalcDistToPlayer();
void Battle_DispatchEntityEffect();
void Battle_HaltOnPositiveX();
s32 Battle_StepAyaAction();
void Battle_StepCharacterAction();
void Battle_UpdateEntityFacing();
void Entity_SetActionMode();
s32 Gte_Atan2();
void Inv_CheckSlotUsable();
void Pm_StopAll();
s32 Scene_LoadRoomAssets();
extern const Tbl20 D_8001074C;
extern const Tbl40 D_80010760;
extern LargeSymbol D_800942E4_o __asm__("D_800942E4");
extern s8 D_8009CE38;
extern s8 D_8009CE39;
extern s8 D_8009CE3A;
extern s8 D_8009CE3B;
extern u8 D_8009CE3C;
extern LargeSymbol D_8009D1A0_r0 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_w0 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_r1 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_w1 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_r2 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_w2 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_r3 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_w3 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_r4 __asm__("D_8009D1A0");
extern LargeSymbol D_8009D1A0_w4 __asm__("D_8009D1A0");
extern u8 D_8009D1D4;
extern LargeSymbol D_8009D20C_o __asm__("D_8009D20C");
extern LargeSymbol D_8009D254_0 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_1 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_2 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_3 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_4 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_5 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_6 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_7 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_8 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_9 __asm__("D_8009D254");
extern void *D_8009D254_10 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_11 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_12 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_13 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_14 __asm__("D_8009D254");
extern LargeSymbol D_8009D254_15 __asm__("D_8009D254");
extern s32 D_8009D258;
extern LargeSymbol D_8009D278_0 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_1 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_2 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_3 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_4 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_5 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_6 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_7 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_8 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_9 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_10 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_11 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_12 __asm__("D_8009D278");
extern LargeSymbol D_8009D278_13 __asm__("D_8009D278");
extern s16 D_8009D27C;
extern LargeSymbol D_8009D2A0_o __asm__("D_8009D2A0");
extern u8 D_800BE830[];
extern QueueCommand D_800BE834[];

#define D_800942E4 (*(u8 **)&D_800942E4_o)
#define D1A0_R0 (*(s32 *)&D_8009D1A0_r0)
#define D1A0_W0 (*(s32 *)&D_8009D1A0_w0)
#define D1A0_R1 (*(s32 *)&D_8009D1A0_r1)
#define D1A0_W1 (*(s32 *)&D_8009D1A0_w1)
#define D1A0_R2 (*(s32 *)&D_8009D1A0_r2)
#define D1A0_W2 (*(s32 *)&D_8009D1A0_w2)
#define D1A0_R3 (*(s32 *)&D_8009D1A0_r3)
#define D1A0_W3 (*(s32 *)&D_8009D1A0_w3)
#define D1A0_R4 (*(s32 *)&D_8009D1A0_r4)
#define D1A0_W4 (*(s32 *)&D_8009D1A0_w4)
#define D_8009D20C (*(void **)&D_8009D20C_o)
#define D254(n) (*(void **)&D_8009D254_##n)
#define D278(n) (*(void **)&D_8009D278_##n)
#define D_8009D2A0 (*(s8 *)&D_8009D2A0_o)

void Battle_AdvancePhase(void) {
    Tbl20 sp18;
    Tbl40 sp30;
    M2C_UNK *temp_v1_2;
    M2C_UNK *var_a2;
    M2C_UNK *var_a3;
    s16 temp_v0;
    s32 var_a2_2;
    s32 var_a3_2;
    s32 var_v0_5;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_s0;
    s32 action;
    s32 command_value;
    s32 actor_flags;
    s32 character_state;
    s32 system_flags;
    FlagValue cleanup_flags;
    FlagValue loop_limit;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a0_2;
    s32 var_s0_2;
    s32 var_s0_4;
    s32 var_v0_2;
    s32 var_v0_6;
    s32 var_v1;
    s8 *temp_v0_5;
    s8 var_a0;
    u16 temp_a0;
    u16 var_a0_3;
    u32 temp_a2;
    u32 end_slot;
    u32 var_v1_2;
    u8 temp_v0_6;
    s32 var_s0;
    u8 var_s0_3;
    s32 var_v0_3;
    void **temp_v1_5;
    void *temp_a0_2;
    QueueEntry *temp_s2;
    void *temp_t0;
    void *temp_v0_4;
    void *temp_v1;
    void *temp_v1_6;
    void *temp_v1_8;
    void *var_a0_4;
    void *var_a1;
    void *actor;
    void *aya_actor;
    void *aya_player;
    u8 *copy_dst;
    const u8 *copy_src;
    const u8 *copy_end;

    var_a0 = D_8009D2A0;
    temp_a2 = D_8009D1D4;
    temp_s2 = (QueueEntry *)(((temp_a2 & 0xFF) * 8) + D_800BE830);
    if (var_a0 == 0) {
        var_s0 = temp_a2;
        if ((temp_a2 & 0xFF) < (u8) D_8009CE3C) {
            do {
                temp_a0 = D_800BE834[var_s0 & 0xFF].value;
                if ((u32) (temp_a0 - 3) < 0x180U) {
                    Inv_CheckSlotUsable((s16) temp_a0 - 3);
                }
                var_s0 += 1;
            } while ((u32) (var_s0 & 0xFF) < (u8) D_8009CE3C);
        }
        D_8009CE3C = 0;
        D_8009D1D4 = 0;
        D1A0_W0 = D1A0_R0 & ~0x100;
        return;
    }
    if (M2C_FIELD(D278(0), s32 *, 0x4C) & 0x200000) {
        sp18 = D_8001074C;
        copy_dst = sp30.bytes;
        copy_src = D_80010760.bytes;
        copy_end = copy_src + 0x20;
        if (((u32)copy_dst | (u32)copy_src) & 3) {
            do {
                *(PackedCopy16 *)copy_dst = *(const PackedCopy16 *)copy_src;
                copy_src += 0x10;
                copy_dst += 0x10;
            } while (copy_src != copy_end);
            *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
            command_value = temp_s2->command;
            goto after_command;
        }
        goto aligned_copy;
block_found:
        var_a2 = temp_v1_2;
        var_a3 = (M2C_UNK *)1;
        goto block_search_done;
aligned_copy:
        do {
            *(Copy16 *)copy_dst = *(const Copy16 *)copy_src;
            copy_src += 0x10;
            copy_dst += 0x10;
        } while (copy_src != copy_end);
copy_tail:
        *(PackedCopy8 *)copy_dst = *(const PackedCopy8 *)copy_src;
        command_value = temp_s2->command;
after_command:
        action = command_value - 0x183;
        var_a0_2 = temp_s2->entity;
        var_a1 = D254(0);
        if (var_a0_2 != var_a1) {
            temp_v0 = Battle_CalcAngleToTarget(var_a0_2 + 0x1B4, var_a1 + 0x28);
            var_a0_2 = (s32) D_8009D1D4;
            var_a1 = (void *) (var_a0_2 & 0xFF);
            var_a2 = (M2C_UNK *) ((s32) var_a1 * 8);
            M2C_FIELD(D254(1), s16 *, 0x3A) = temp_v0;
            var_a3 = NULL;
            if (*(s16 *)((u8 *)D_800BE834 + (s32)var_a2) - 0x183 == 0x13) {
                var_a2 = NULL;
                var_s0 = var_a0_2;
                temp_v1 = var_a1 + 7;
                if ((s32) var_a1 < (s32) temp_v1) {
                    loop_limit.value = (s32) temp_v1;
loop_17:
                    temp_v1_2 = *(void **)(D_800BE830 + ((var_s0 & 0xFF) * 8));
                    if ((temp_v1_2 == NULL) || (M2C_FIELD(M2C_FIELD(temp_v1_2, void **, 0), s32 *, 0x10) <= 0) || (M2C_FIELD(temp_v1_2, s32 *, 0x98) & 0x4000)) {
                        var_s0 += 1;
                        if ((var_s0 & 0xFF) < loop_limit.value) {
                            goto loop_17;
                        }
                    } else {
                        goto block_found;
                    }
                }
block_search_done:
                if (!((s32) var_a3 & 0xFF)) {
                    var_v0_3 = D_8009D1D4 + 7;
                    goto block_35;
                }
                if (var_a2 != NULL) {
                    var_s0 = D_8009D1D4;
                    temp_v1_3 = var_s0 & 0xFF;
                    var_a0_2 = temp_v1_3 + 7;
                    if (temp_v1_3 < var_a0_2) {
                        var_a1 = (void *) var_a0_2;
                        do {
                            var_a0_2 = temp_v1_3 * 8;
                            temp_v1_5 = *(void ***)(D_800BE830 + var_a0_2);
                            if ((M2C_FIELD(*temp_v1_5, s32 *, 0x10) <= 0) || (temp_v1_5 == NULL)) {
                                *(void **)(D_800BE830 + var_a0_2) = var_a2;
                            }
                            var_s0 += 1;
                            temp_v1_3 = var_s0 & 0xFF;
                        } while (temp_v1_3 < (s32) var_a1);
                    }
                }
                goto block_39;
            }
            temp_v1_6 = *(void **)(D_800BE830 + (s32)var_a2);
            if ((temp_v1_6 == NULL) || (M2C_FIELD(M2C_FIELD(temp_v1_6, void **, 0), s32 *, 0x10) <= 0) || (M2C_FIELD(temp_v1_6, s32 *, 0x98) & 0x4000)) {
                var_v0_3 = D_8009D1D4 + 1;
block_35:
                temp_v1_3 = var_v0_3 & 0xFF;
                temp_v1_3 <<= 3;
                temp_v1_3 = *(u16 *)((u8 *)D_800BE834 + temp_v1_3);
                D_8009D1D4 = var_v0_3;
                temp_v1_3 -= 3;
                temp_v1_3 = (u32) temp_v1_3 < 0x194U;
                if (!temp_v1_3 || ((u8) M2C_FIELD(D254(2), u8 *, 0xE) < 4U)) {
                    D1A0_W1 = D1A0_R1 & ~0x100;
                }
                Entity_SetActionMode(D254(3), M2C_FIELD(D278(1), u8 *, 0x12), var_a2, (s32) var_a3);
                M2C_FIELD(D278(2), s32 *, 0x4C) = (s32) (M2C_FIELD(D278(2), s32 *, 0x4C) | 0x200000);
                return;
            }
            goto block_39;
        }
block_39:
        Pm_StopAll();
        if (action == 6) {
            temp_a0_2 = M2C_FIELD(D278(3), void **, 0x68);
            if (!(M2C_FIELD(temp_a0_2, s32 *, 0xC) & 0x3FF)) {
                BattleCmd_CommitAmmoAndUpdate(temp_a0_2);
            }
            M2C_FIELD(D278(4), s32 *, 0x4C) = (s32) (M2C_FIELD(D278(4), s32 *, 0x4C) | 0x100000);
            temp_a0_3 = M2C_FIELD(temp_s2, s32 *, 0);
            temp_s0 = M2C_FIELD(temp_a0_3, s16 *, 0x26A) - D_8009D27C;
            temp_v0_2 = Gte_Atan2(temp_s0, Battle_CalcDistToPlayer((void *) temp_a0_3, D254(4)));
            if (temp_v0_2 < -0xAB) {
                var_v1 = 0;
            } else {
                var_v1 = 2;
                if (temp_v0_2 < 0xE4) {
                    var_v1 = 1;
                }
            }
            Entity_SetActionMode(D254(5), M2C_FIELD((void *)((s32)var_v1 + (s32)D278(5)), u8 *, 0x14));
            M2C_FIELD(D278(6), s32 *, 0x4C) = (s32) (M2C_FIELD(D278(6), s32 *, 0x4C) & 0xFFDFFFFF);
            D_8009CE39 = 0x58;
            D_8009CE38 = 0;
            D_8009CE3A = 0x18;
            D_8009CE3B = 0;
        } else {
            Entity_SetActionMode(D254(6), 0xFU);
        }
        if (action != 0x13) {
            D_8009D258 = Scene_LoadRoomAssets(sp18.bytes[action], (void *) M2C_FIELD(temp_s2, s32 *, 0));
        }
        if (((u32) (action - 7) < 2U) || (action == 0xA)) {
            temp_v0_3 = M2C_FIELD(temp_s2, s32 *, 0);
            var_a0_3 = *(u16 *)&sp30.bytes[action * 2];
            var_a2_2 = M2C_FIELD(temp_v0_3, s16 *, 0x268);
            var_a3_2 = M2C_FIELD(temp_v0_3, s16 *, 0x26A);
            var_v0_5 = M2C_FIELD(temp_v0_3, s16 *, 0x26C);
        } else {
            var_a0_3 = *(u16 *)&sp30.bytes[action * 2];
            var_a2_2 = M2C_FIELD(D254(7), s16 *, 0x2A);
            var_a3_2 = M2C_FIELD(D254(7), s16 *, 0x2E);
            var_v0_5 = M2C_FIELD(D254(7), s16 *, 0x32);
        }
        Akao_SendPositionalCmdStereo(var_a0_3, 0, var_a2_2, var_a3_2, var_v0_5);
        var_a1 = (void *)0xFFDFFFFF;
        var_a0_4 = D278(7);
        actor_flags = M2C_FIELD(var_a0_4, s32 *, 0x4C);
        D1A0_W2 = D1A0_R2 | 0x100;
        actor_flags &= (s32)var_a1;
        M2C_FIELD(var_a0_4, s32 *, 0x4C) = actor_flags;
        goto block_55;
    }
block_55:
    actor = D278(8);
    actor_flags = M2C_FIELD(actor, s32 *, 0x4C);
    if (actor_flags & 0x80000) {
        if ((Battle_StepAyaAction() << 0x18) != 0) {
            end_slot = D_8009D1D4;
            aya_actor = D278(9);
            aya_player = D254(8);
            M2C_FIELD(aya_actor, s32 *, 8) = 0;
            M2C_FIELD(aya_player, s32 *, 0x98) = (s32) (M2C_FIELD(aya_player, s32 *, 0x98) & ~0x100);
            var_s0_4 = end_slot - 7;
            var_a2_2 = var_s0_4 & 0xFF;
            if ((u32)var_a2_2 < end_slot) {
                do {
                    var_v0_6 = var_a2_2 * 8;
                    temp_a0_4 = *(s32 *)(D_800BE830 + var_v0_6);
                    if (temp_a0_4 != D254(9)) {
                        Battle_UpdateEntityFacing(temp_a0_4);
                    }
                    var_s0_4 += 1;
                    var_a2_2 = var_s0_4 & 0xFF;
                } while ((u32)var_a2_2 < (u8) D_8009D1D4);
            }
            var_a0_4 = D_8009D20C;
            if (var_a0_4 != NULL) {
                do {
                    if (var_a0_4 != D_8009D254_10) {
                        temp_v0_4 = M2C_FIELD(var_a0_4, void **, 0);
                        if (temp_v0_4 != NULL) {
                            temp_v0_5 = M2C_FIELD(temp_v0_4, s8 **, 0x18);
                            if (temp_v0_5 != NULL) {
                                *temp_v0_5 = 4;
                                temp_v1_8 = M2C_FIELD(var_a0_4, void **, 0);
                                M2C_FIELD(temp_v1_8, s32 *, 0) = (s32) (M2C_FIELD(temp_v1_8, s32 *, 0) & 0xC0FFFFFF);
                            }
                        }
                    }
                    var_a0_4 = M2C_FIELD(var_a0_4, void **, 4);
                } while (var_a0_4 != NULL);
            }
            M2C_FIELD(D278(10), s32 *, 0x4C) = (s32) (M2C_FIELD(D278(10), s32 *, 0x4C) & 0xFFF7FFFF);
            Battle_HaltOnPositiveX();
        }
    } else if (actor_flags & 0x100000) {
        temp_v0 = Battle_CalcAngleToTarget(M2C_FIELD(temp_s2, s32 *, 0) + 0x1B4, D254(11) + 0x28);
        M2C_FIELD(D254(12), s16 *, 0x3A) = temp_v0;
        Battle_StepCharacterAction(temp_s2);
        if ((u32) (*((D_8009D258 * 0xA0C) + D_800942E4) - 1) >= 2U) {
            var_a2_2 = 0xFFEFFFFF;
            temp_a0_3 = -0x101;
            var_a1 = D278(11);
            character_state = (s32)D254(13);
            M2C_FIELD(var_a1, s32 *, 8) = 0;
            M2C_FIELD((void *)character_state, s32 *, 0x98) = (s32) (M2C_FIELD((void *)character_state, s32 *, 0x98) & temp_a0_3);
            system_flags = D1A0_R3;
            cleanup_flags.value = M2C_FIELD(var_a1, s32 *, 0x4C);
            system_flags &= temp_a0_3;
            cleanup_flags.value &= var_a2_2;
            D1A0_W3 = system_flags;
            cleanup_flags.value |= 0x200000;
            M2C_FIELD(var_a1, s32 *, 0x4C) = cleanup_flags.value;
            Battle_HaltOnPositiveX();
        }
    } else {
        if (M2C_FIELD(D254(14), u8 *, 0xF) == M2C_FIELD(D254(14), u16 *, 0x1A)) {
            Entity_SetActionMode(D254(14), M2C_FIELD(actor, u8 *, 0x12));
        }
        if ((((u32) (*((D_8009D258 * 0xA0C) + D_800942E4) - 1) >= 2U) || (M2C_FIELD(temp_s2, s16 *, 4) == 0x196)) && (M2C_FIELD(D278(13), s32 *, 0x4C) = (s32) (M2C_FIELD(D278(13), s32 *, 0x4C) | 0x200000), Battle_ApplySpellEffect(M2C_FIELD(temp_s2, s16 *, 4) - 0x183, (void *) M2C_FIELD(temp_s2, s32 *, 0)), (M2C_FIELD(temp_s2, s16 *, 4) != 0x196))) {
            temp_v0_6 = D_8009D1D4 + 1;
            D_8009D1D4 = temp_v0_6;
            if (((u32) (D_800BE834[temp_v0_6 & 0xFF].value - 3) >= 0x194U) || ((u8) M2C_FIELD(D254(15), u8 *, 0xE) < 4U)) {
                D1A0_W4 = D1A0_R4 & ~0x100;
            }
            temp_a0_4 = (u8)D_8009CE3C;
            for (var_s0_4 = D_8009D1D4; (u32)(var_s0_4 & 0xFF) < (u32)temp_a0_4; var_s0_4++) {
                if ((u32)(D_800BE834[var_s0_4 & 0xFF].value - 1) < 2U) {
                    Battle_DispatchEntityEffect();
                    return;
                }
            }
        }
    }
}
