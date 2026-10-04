/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
extern u8 D_8009D274;
int rand(void);
int Gte_Atan2(int y, int x);
extern Combatant *view_D_8009D278[16] asm("D_8009D278");
#define D_8009D278 view_D_8009D278[0]
extern BattleEntity *view_D_8009D254[16] asm("D_8009D254");
#define D_8009D254 view_D_8009D254[0]
extern BattleEntity *view_D_8009D20C[16] asm("D_8009D20C");
#define D_8009D20C view_D_8009D20C[0]
extern u8 view_D_8009D1CE[16] asm("D_8009D1CE");
#define D_8009D1CE view_D_8009D1CE[0]
extern void *view_D_8009D1F8[16] asm("D_8009D1F8");
#define D_8009D1F8 view_D_8009D1F8[0]
u8 D_8009D274, D_8009D1D4, D_8009D294;
s8 D_8009CE54, D_8009CE55;
typedef struct HitFlagBits {
    unsigned low : 13;
    unsigned actionPhase : 2;
    unsigned reaction : 3;
    unsigned attackClass : 2;
    unsigned high : 12;
} HitFlagBits;
void Battle_ResolveHitOnTimer(void) {
    register BattleEntity *resultActor asm("$3");
    register s32 hitStatus asm("$5");
    BattleAction *temp_a1;
    BattleEntity *temp_a0;
    BattleEntity *temp_a0_4;
    BattleEntity *temp_v0_2;
    register BattleEntity *var_s0;
    register BattleEntity *var_s0_2;
    register BattleInitSlot *temp_s1;
    EnemyCombatant *temp_a1_2;
    register EnemyCombatant *temp_a2;
    register EnemyCombatant *temp_a2_2;
    register EnemyCombatant *temp_a2_3;
    register EnemyCombatant *temp_a2_4;
    register EnemyCombatant *var_a2;
    s32 temp_v0_3;
    s16 temp_v1;
    s32 var_a0;
    s32 var_v0;
    s32 temp_v1_2;
    s32 temp_v1_8;
    s32 var_v0_2;
    register s8 temp_s2;
    u16 temp_v1_7;
    u32 temp_a0_2;
    u32 temp_a0_3;
    u32 temp_v1_10;
    u32 temp_v1_11;
    u32 temp_v1_12;
    u32 temp_v1_3;
    u32 temp_v1_4;
    u32 temp_v1_5;
    u32 temp_v1_6;
    u32 temp_v1_9;
    u8 temp_v0;

    temp_v0 = D_8009D274 + 1;
    temp_a1 = D_8009D278->action;
    temp_s1 = &D_800BE830[D_8009D1D4];
    D_8009D274 = temp_v0;
    if ((temp_v0 & 0xFF) == temp_a1->field08) {
        temp_v1 = temp_a1->actionCode.actionId;
        if (temp_v1 == 8) {
            Battle_CheckEvasion(temp_s1->actor, (BattleEvasionOutcome *)&D_8009CE54, 0);
            if (D_8009CE54 == 1) {
                temp_v0_2 = temp_s1->actor;
                Asset_Find08Alt(0x46D, 0, (s32) temp_v0_2->renderObject.target_x, (s32) temp_v0_2->renderObject.target_y, (s32) temp_v0_2->renderObject.target_z);
                if (D_8009D278->action->turnWord & 0x6000) {
                    temp_v1_2 = ((u32) ((EnemyCombatant *)temp_s1->actor->core)->statusFlags2 >> 0xC) & 3;
                    switch (temp_v1_2) {            /* irregular */
                    case 0:
                        if (!(rand() & 1)) {
                        case 2:
                            if (Inv_AddItem((s32) ((EnemyCombatant *)temp_s1->actor->core)->stealItemId) != 0) {
                                D_8009D1CE = 1;
                                D_8009D1F8 = Inv_GetItemEffectData((s32) ((EnemyCombatant *)temp_s1->actor->core)->stealItemId, 2);
                            } else {
                                D_8009D1CE = 1;
                                D_8009D1F8 = Inv_GetItemEffectData((s32) ((EnemyCombatant *)temp_s1->actor->core)->stealItemId, 1);
                                ((EnemyCombatant *)temp_s1->actor->core)->stealItemId = 0;
                            }
                        }
                        break;
                    }
                }
            }
            resultActor = temp_s1->actor;
            hitStatus = D_8009CE54;
            var_a2 = resultActor->core;
            asm("" : : "r"(resultActor), "r"(hitStatus));
            if (hitStatus != 1) {
                goto block_61;
            }
            goto block_60;
        }
        if (temp_v1 == 6) {
            Battle_CheckEvasion(temp_s1->actor, (BattleEvasionOutcome *)&D_8009CE54, 0);
            temp_a0 = temp_s1->actor;
            temp_a2 = temp_a0->core;
            if (D_8009CE54 == 1) {

                    ((HitFlagBits *)temp_a2)->actionPhase = 1;
                    ((HitFlagBits *)temp_a2)->attackClass = D_8009D278->action->attackWord >> 20;
                    ((HitFlagBits *)temp_a2)->reaction = D_8009CE55;
            } else if (D_8009CE54 == 0) {
                temp_a2->panelC_val = -1;
                temp_a2->panelC_timer = 0x1E;
                temp_a2->panelC_x = (u16) temp_a0->renderObject.projected_target_x;
                temp_a2->panelC_y = (u16) temp_a0->renderObject.projected_target_y - 0x14;
            }
            temp_s2 = (s8) (u8) D_8009CE55;
            if (D_8009CE54 == 1) {
                var_s0 = D_8009D20C;
                if (var_s0 != 0) {
loop_20:
                    if (var_s0 != D_8009D254) {
                        temp_a1_2 = var_s0->core;
                        if (temp_a1_2 != 0) {
                            temp_a0_2 = var_s0->entityFlags;
                            if (((temp_a0_2 & 0x2040) != 0x40) && !(temp_a0_2 & 0x4000) && (temp_a1_2->hpAlive > 0) && (var_s0 != temp_s1->actor)) {
                                Battle_CheckEvasion(var_s0, (BattleEvasionOutcome *)&D_8009CE54, (s32) (s16) temp_s2);
                                temp_a2_2 = var_s0->core;
                                if (D_8009CE54 == 1) {
                                    {

                    ((HitFlagBits *)temp_a2_2)->actionPhase = 1;
                    ((HitFlagBits *)temp_a2_2)->attackClass = D_8009D278->action->attackWord >> 20;
                    ((HitFlagBits *)temp_a2_2)->reaction = D_8009CE55;
                    }
                                } else if (D_8009CE54 == 0) {
                                    temp_a2_2->panelC_val = -1;
                                    temp_a2_2->panelC_timer = 0x1E;
                                    temp_a2_2->panelC_x = (u16) var_s0->renderObject.projected_target_x;
                                    temp_a2_2->panelC_y = (u16) var_s0->renderObject.projected_target_y - 0x14;
                                }
                            }
                        }
                    }
                    var_s0 = var_s0->next;
                    if (var_s0 != 0) {
                        goto loop_20;
                    }
                }
            }
        } else if (((temp_a1->turnWord & 0xC0) == 0x80) && !(D_8009D278->stateFlags & 0x100000)) {
            temp_v1_7 = (u16) D_8009D254->facingAngle;
            temp_v0_3 = temp_v1_7 - 0x800;
            var_v0 = temp_v1_7 + 0x600;
            if ((s16)temp_v0_3 < -0x600) {
                var_a0 = temp_v1_7 - 0x600;
            } else {
                var_a0 = temp_v1_7 - 0xA00;
                if ((s16)temp_v0_3 < 0x600) {
                    var_v0 = temp_v1_7 - 0x600;
                } else {
                    var_a0 = temp_v1_7 - 0x1600;
                    var_v0 = temp_v1_7 - 0xA00;
                }
            }
            var_s0_2 = D_8009D20C;
            if (var_s0_2 != 0) {
                register s32 lower = (s16)var_a0;
                register s32 upper = (s16)var_v0;
                register s32 facing = (s16)temp_v0_3;
                register int upperRange = facing < 0x600;
                asm("" : "=r"(upperRange) : "0"(upperRange));
loop_41:
                if (var_s0_2 != D_8009D254) {
                    temp_a2_3 = var_s0_2->core;
                    if (temp_a2_3 != 0) {
                        temp_a0_3 = var_s0_2->entityFlags;
                        if (((temp_a0_3 & 0x2040) != 0x40) && !(temp_a0_3 & 0x4000) && (temp_a2_3->hpAlive > 0)) {
                            temp_v1_8 = Gte_Atan2(var_s0_2->renderObject.target_x - D_8009D254->posX.parts.integer, var_s0_2->renderObject.target_z - D_8009D254->posZ.parts.integer);
                            if (facing >= -0x600 && upperRange) {
                                if ((s16)temp_v1_8 < lower || upper < (s16)temp_v1_8) goto nextConeTarget;
                            } else {
                                if ((s16)temp_v1_8 < upper && lower < (s16)temp_v1_8) goto nextConeTarget;
                            }
                            {
                                    Battle_CheckEvasion(var_s0_2, (BattleEvasionOutcome *)&D_8009CE54, 0);
                                    temp_a2_4 = var_s0_2->core;
                                    if (D_8009CE54 == 1) {
                                        {

                    ((HitFlagBits *)temp_a2_4)->actionPhase = 1;
                    ((HitFlagBits *)temp_a2_4)->attackClass = D_8009D278->action->attackWord >> 20;
                    ((HitFlagBits *)temp_a2_4)->reaction = D_8009CE55;
                    }
                                    } else if (D_8009CE54 == 0) {
                                        temp_a2_4->panelC_val = -1;
                                        temp_a2_4->panelC_timer = 0x1E;
                                        temp_a2_4->panelC_x = (u16) var_s0_2->renderObject.projected_target_x;
                                        temp_a2_4->panelC_y = (u16) var_s0_2->renderObject.projected_target_y - 0x14;
                                    }
                            }
                        }
                    }
                }
nextConeTarget:
                var_s0_2 = var_s0_2->next;
                if (var_s0_2 != 0) {
                register s32 lower = (s16)var_a0;
                register s32 upper = (s16)var_v0;
                register s32 facing = (s16)temp_v0_3;
                register int upperRange = facing < 0x600;
                asm("" : "=r"(upperRange) : "0"(upperRange));
                    goto loop_41;
                }
            }
        } else {
            temp_a0_4 = temp_s1->actor;
            if (temp_a0_4 != 0) {
                Battle_CheckEvasion(temp_a0_4, (BattleEvasionOutcome *)&D_8009CE54, 0);
                resultActor = temp_s1->actor;
                hitStatus = D_8009CE54;
                var_a2 = resultActor->core;
                if (hitStatus == 1) {
block_60:
                    {

                    ((HitFlagBits *)var_a2)->actionPhase = 1;
                    ((HitFlagBits *)var_a2)->attackClass = D_8009D278->action->attackWord >> 20;
                    ((HitFlagBits *)var_a2)->reaction = D_8009CE55;
                    }
                } else {
block_61:
                    if (hitStatus == 0) {
                        var_a2->panelC_val = -1;
                        var_a2->panelC_timer = 0x1E;
                        var_a2->panelC_x = resultActor->renderObject.projected_target_x;
                        var_a2->panelC_y = resultActor->renderObject.projected_target_y - 0x14;
                    }
                }
            }
        }
        D_8009D294 = 0;
        D_8009D274 = 0;
    }
}
