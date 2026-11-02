/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/battle.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_setup.h"
#include "pe1/battle_status.h"
#include "pe1/battle_update.h"
#include "pe1/draw_slot_record.h"
#include "pe1/entity_animation.h"
#include "pe1/geom_state.h"
#include "pe1/gte_types.h"
#include "pe1/menu_flags.h"
#include "pe1/menu_widget.h"
#include "pe1/pm.h"
#include "pe1/scene_transition.h"
#include "pe1/textbox.h"
#include "m2c_macros.h"

#define NULL ((void *)0)

s8 Pad_GetMenuPressedBitOrDisabled(void);
struct BattleStatusPanel;
void Battle_DrawStatusPanel(s32, struct BattleStatusPanel *);
M2C_UNK Entity_ResolveDropTable(void *);                      /* static */
s16 Entity_ApplyHitAndSetAnim(void *);                 /* static */
void Entity_UpdateTimers(void);
/* Separate declarations preserve the original GCC quantities for each tail. */
/* COMMON supplies small-data metadata to MASPSX. */
s8 g_BattleHaltTimer;
unsigned char g_AyaStatusEffectTimer;


static inline u8 Battle_GetReactionTimer(void) {
    return g_BattleEnemyHoldTimer[0];
}

static inline void Battle_SetReactionTimer(u8 value) {
    g_BattleEnemyHoldTimer[0] = value;
}

void Menu_MainUpdate(s32 arg0) {
    char unused_stack[0x2C0];
    s16 temp_v0_7;
    s16 temp_v0_8;
    s16 temp_v1_10;
    s16 temp_v1_11;
    s16 temp_v1_13;
    s16 temp_v1_8;
    s32 *temp_s2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_4;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 temp_v1_9;
    register s32 var_a2;
    register s32 var_a1;
    register s32 var_a3;
    register union {
        s32 value;
        BattleEntity *entity;
    } var_t0;
    register s32 var_t1;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    register s32 var_v1;
    register u32 var_a0;
    register u8 var_a0_2;
    u16 temp_a1;
    register union {
        u32 value;
        BattleEntity *entity;
    } var_v0;
    u32 temp_a2;
    EnemyActionEffect *temp_a0_2;
    u8 temp_a0;
    u8 temp_v0_6;
    u8 temp_v1_12;
    register Combatant *state0;
    register Combatant *temp_a1_2;
    EnemyCombatant *temp_v1_5;
    BattleEntity *var_s0;

    state0 = g_ActiveActor;
    temp_s2 = &state0->stateFlags;
    if (arg0 & 0xFF) {
        if (!(g_GameStateFlags & 0x100)) {
            temp_a2 = state0->stateFlags;
            temp_a1 = state0->hpAlive + state0->scaledOffense;
            temp_v1 = temp_a2 & 0xC0;
            state0->hpAlive = temp_a1;
            if ((temp_v1 == 0x40) || (temp_v1 == 0x80)) {
                state0->hpAlive = temp_a1 - ((state0->scaledOffense * 2) / 5);
            } else if (temp_a2 & 0x100) {
                state0->hpAlive = temp_a1 + (state0->scaledOffense >> 1);
            }
            temp_a1_2 = g_ActiveActor;
            if ((temp_a1_2->exp_or_acc < temp_a1_2->maxAtk) && !(*temp_s2 & 0x2600)) {
                temp_v0 = temp_a1_2->atbRate - 3;
                temp_a1_2->atbRate = temp_v0;
                if (temp_v0 <= 0) {
                    temp_a1_2->atbRate = 1;
                }
                temp_v0_2 = temp_a1_2->atbStep - (temp_a1_2->maxAtk / (temp_a1_2->atbRate * 0x64));
                temp_a1_2->atbStep = temp_v0_2;
                if (temp_v0_2 < 0x1999) {
                    temp_a1_2->atbStep = 0x1999;
                }
                temp_v0_3 = g_ActiveActor->exp_or_acc + temp_a1_2->atbStep;
                g_ActiveActor->exp_or_acc = temp_v0_3;
                if (temp_v0_3 >= temp_a1_2->maxAtk) {
                    temp_a1_2->atbGauge = 0xF0;
                    if (g_AkaoBgmHandle[0] != 0) {
                        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x455, 0, 0x80, 0x7F);
                    }
                }
            }
        }
        if ((arg0 & 0xFF) == 1) {
            temp_a0 = D_8009CD10->actionMode;
            if (((temp_a0 == g_ActiveActor->actionMode12) || (temp_a0 == 5)) &&
                !((g_ActiveActor->attributes->effectFlags & 0x4000) && (g_ActiveActor->stateFlags & 0x4000) &&
                  (Pad_GetMenuPressedBitOrDisabled() << 0x18) > 0)) {
                D_8009CDA8 &= ~1;
            } else if (D_8009CD10->actionMode >= 0xEU) {
                D_8009CDA8 &= ~1;
            } else {
                D_8009CDA8 |= 1;
            }
        }
    } else {
        D_8009CDA8 |= 1;
    }
    if (g_ActiveActor->exp_or_acc < 0) {
        g_ActiveActor->exp_or_acc = 0;
    }
    if (*temp_s2 & 0x2000) {
        var_v1 = g_BattleFrameCounter & 3;
        if (var_v1 == 0) {
            var_t1 = 0xFF;
            var_t0.value = 0x3D;
            var_a3 = 0x81;
            var_a2 = 0x83;
            var_a1 = 0x13;
            {
            asm volatile("");
            var_v1 = g_ActiveDrawSlot;
            do { } while (0);
            var_a0 = 1;
            g_BattlePeGauges[var_v1][1].r0 = var_t1;
            }
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = var_t0.value;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = var_a3;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = var_t1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = var_t0.value;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = var_a3;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = var_a1;
            var_v0_3 = g_ActiveDrawSlotRecord.slot;
            g_BattlePeGauges[var_v0_3][1].b3 = var_a0;
        }
        else if (var_v1 == 1) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            asm volatile("");
            var_v1 = g_ActiveDrawSlot;
            do { } while (0);
            var_a0 = 0x41;
            g_BattlePeGauges[var_v1][1].r0 = var_a2;
            }
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = var_a1;
            var_v0_3 = g_ActiveDrawSlotRecord.slot;
            g_BattlePeGauges[var_v0_3][1].b3 = var_a0;
        }
        else if (var_v1 == 2) {
            var_t1 = 0x83;
            var_t0.value = 0x13;
            var_a3 = 1;
            var_a2 = 0xFF;
            var_a1 = 0x3D;
            {
            asm volatile("");
            var_v1 = g_ActiveDrawSlot;
            do { } while (0);
            var_a0 = 0x81;
            g_BattlePeGauges[var_v1][1].r0 = var_t1;
            }
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = var_t0.value;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = var_a3;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = var_t1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = var_t0.value;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = var_a3;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = var_a1;
            var_v0_3 = g_ActiveDrawSlotRecord.slot;
            g_BattlePeGauges[var_v0_3][1].b3 = var_a0;
        }
        else {
        var_a2 = 0xC1;
        if (var_v1 == 3) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            asm volatile("");
            var_v1 = g_ActiveDrawSlot;
            do { } while (0);
            var_a0 = 0x41;
            g_BattlePeGauges[var_v1][1].r0 = var_a2;
            }
            g_BattlePeGauges[g_ActiveDrawSlot][1].g0 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b0 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r1 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g1 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b1 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r2 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g2 = var_a1;
            g_BattlePeGauges[g_ActiveDrawSlot][1].b2 = var_a0;
            g_BattlePeGauges[g_ActiveDrawSlot][1].r3 = var_a2;
            g_BattlePeGauges[g_ActiveDrawSlot][1].g3 = var_a1;
            var_v0_3 = g_ActiveDrawSlot;
            g_BattlePeGauges[var_v0_3][1].b3 = var_a0;
        }
        }
        if (D_8009CD10->actionMode != 0x12) {
            ((void (*)(BattleEntity *, int, int))Entity_SetActionMode)(D_8009CD10, 0x12U, var_a2);
        }
        if (g_BattleHaltTimer == 0x5A) {
            s32 reset_flags;
            u32 reset_operand;

            Entity_SetActionMode(D_8009CD10, g_ActiveActor->actionMode12);
            reset_flags = *temp_s2;
            reset_operand = ~0x2000U;
            reset_flags &= reset_operand;
            reset_operand = ((union { Combatant *actor; u32 word; } *)&g_ActiveActor)->word;
            g_BattleHaltTimer = 0;
            *temp_s2 = reset_flags;
            ((Combatant *)reset_operand)->exp_or_acc = 0x10000;
            g_BattlePeGauges[0][1].r0 = 0xFF;
            g_BattlePeGauges[0][1].g0 = 0x3D;
            g_BattlePeGauges[0][1].b0 = 0x81;
            g_BattlePeGauges[0][1].r1 = 0x83;
            g_BattlePeGauges[0][1].g1 = 0x13;
            g_BattlePeGauges[0][1].b1 = 1;
            g_BattlePeGauges[0][1].r2 = 0xFF;
            g_BattlePeGauges[0][1].g2 = 0x3D;
            g_BattlePeGauges[0][1].b2 = 0x81;
            g_BattlePeGauges[0][1].r3 = 0x83;
            g_BattlePeGauges[0][1].g3 = 0x13;
            g_BattlePeGauges[0][1].b3 = 1;
            g_BattlePeGauges[1][1].r0 = 0xFF;
            g_BattlePeGauges[1][1].g0 = 0x3D;
            g_BattlePeGauges[1][1].b0 = 0x81;
            g_BattlePeGauges[1][1].r1 = 0x83;
            g_BattlePeGauges[1][1].g1 = 0x13;
            g_BattlePeGauges[1][1].b1 = 1;
            g_BattlePeGauges[1][1].r2 = 0xFF;
            g_BattlePeGauges[1][1].g2 = 0x3D;
            g_BattlePeGauges[1][1].b2 = 0x81;
            g_BattlePeGauges[1][1].r3 = 0x83;
            g_BattlePeGauges[1][1].g3 = 0x13;
            g_BattlePeGauges[1][1].b3 = 1;
        } else {
            g_BattleHaltTimer += 1;
            D_8009CD10->motionX = 0;
            D_8009CD10->motionY = 0;
            D_8009CD10->motionZ = 0;
        }
    }
    if (!(g_GameStateFlags & 0x100)) {
        register Combatant *timer_state;

        timer_state = g_ActiveActor;
        temp_v0_4 = timer_state->atbGauge;
        if (temp_v0_4 > 0) {
            temp_v0_4 -= 1;
            timer_state->atbGauge = temp_v0_4;
            if (temp_v0_4 != 0) {
                var_v1 = g_BattleFrameCounter & 3;
                if (var_v1 == 0) {
                    var_t0.value = 0x82;
                    var_a3 = 0x36;
                    var_a2 = 0x4A;
                    var_a1 = 0xFF;
                    {
                    asm volatile("");
                    var_v1 = g_ActiveDrawSlot;
                    do { } while (0);
                    var_a0_2 = 0x3B;
                    g_BattlePeGauges[var_v1][0].r0 = 0;
                    }
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g0 = var_t0.value;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b0 = var_a3;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r1 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g1 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b1 = 0x3B;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r2 = 0;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g2 = var_t0.value;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b2 = var_a3;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r3 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g3 = var_a1;
                    var_v0_4 = g_ActiveDrawSlotRecord.slot;
                    g_BattlePeGauges[var_v0_4][0].b3 = var_a0_2;
                }
                else if (var_v1 == 1) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    asm volatile("");
                    var_v1 = g_ActiveDrawSlot;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    g_BattlePeGauges[var_v1][0].r0 = var_a2;
                    }
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g0 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b0 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r1 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g1 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b1 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r2 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g2 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b2 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r3 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g3 = var_a1;
                    var_v0_4 = g_ActiveDrawSlotRecord.slot;
                    g_BattlePeGauges[var_v0_4][0].b3 = var_a0_2;
                }
                else if (var_v1 == 2) {
                    var_t0.value = 0x4A;
                    var_a3 = 0xFF;
                    var_a2 = 0x3B;
                    var_a1 = 0x82;
                    {
                    asm volatile("");
                    var_v1 = g_ActiveDrawSlot;
                    do { } while (0);
                    var_a0_2 = 0x36;
                    g_BattlePeGauges[var_v1][0].r0 = var_t0.value;
                    }
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g0 = var_a3;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b0 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r1 = 0;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g1 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b1 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r2 = var_t0.value;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g2 = var_a3;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b2 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r3 = 0;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g3 = var_a1;
                    var_v0_4 = g_ActiveDrawSlotRecord.slot;
                    g_BattlePeGauges[var_v0_4][0].b3 = var_a0_2;
                }
                else if (var_v1 == 3) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    asm volatile("");
                    var_v1 = g_ActiveDrawSlot;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    g_BattlePeGauges[var_v1][0].r0 = var_a2;
                    }
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g0 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b0 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r1 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g1 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b1 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r2 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g2 = var_a1;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].b2 = var_a0_2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].r3 = var_a2;
                    g_BattlePeGauges[g_ActiveDrawSlot][0].g3 = var_a1;
                    var_v0_4 = g_ActiveDrawSlot;
                    g_BattlePeGauges[var_v0_4][0].b3 = var_a0_2;
                }
                if (g_ActiveActor->exp_or_acc < g_ActiveActor->maxAtk) {
                    g_ActiveActor->atbGauge = 0;
                }
                if (g_ActiveActor->atbGauge == 0) {
                g_BattlePeGauges[0][0].r0 = 0;
                g_BattlePeGauges[0][0].g0 = 0x82;
                g_BattlePeGauges[0][0].b0 = 0x36;
                g_BattlePeGauges[0][0].r1 = 0x4A;
                g_BattlePeGauges[0][0].g1 = 0xFF;
                g_BattlePeGauges[0][0].b1 = 0x3B;
                g_BattlePeGauges[0][0].r2 = 0;
                g_BattlePeGauges[0][0].g2 = 0x82;
                g_BattlePeGauges[0][0].b2 = 0x36;
                g_BattlePeGauges[0][0].r3 = 0x4A;
                g_BattlePeGauges[0][0].g3 = 0xFF;
                g_BattlePeGauges[0][0].b3 = 0x3B;
                g_BattlePeGauges[1][0].r0 = 0;
                g_BattlePeGauges[1][0].g0 = 0x82;
                g_BattlePeGauges[1][0].b0 = 0x36;
                g_BattlePeGauges[1][0].r1 = 0x4A;
                g_BattlePeGauges[1][0].g1 = 0xFF;
                g_BattlePeGauges[1][0].b1 = 0x3B;
                g_BattlePeGauges[1][0].r2 = 0;
                g_BattlePeGauges[1][0].g2 = 0x82;
                g_BattlePeGauges[1][0].b2 = 0x36;
                g_BattlePeGauges[1][0].r3 = 0x4A;
                g_BattlePeGauges[1][0].g3 = 0xFF;
                g_BattlePeGauges[1][0].b3 = 0x3B;
                }
            } else {
                g_BattlePeGauges[0][0].r0 = 0;
                g_BattlePeGauges[0][0].g0 = 0x82;
                g_BattlePeGauges[0][0].b0 = 0x36;
                g_BattlePeGauges[0][0].r1 = 0x4A;
                g_BattlePeGauges[0][0].g1 = 0xFF;
                g_BattlePeGauges[0][0].b1 = 0x3B;
                g_BattlePeGauges[0][0].r2 = 0;
                g_BattlePeGauges[0][0].g2 = 0x82;
                g_BattlePeGauges[0][0].b2 = 0x36;
                g_BattlePeGauges[0][0].r3 = 0x4A;
                g_BattlePeGauges[0][0].g3 = 0xFF;
                g_BattlePeGauges[0][0].b3 = 0x3B;
                g_BattlePeGauges[1][0].r0 = 0;
                g_BattlePeGauges[1][0].g0 = 0x82;
                g_BattlePeGauges[1][0].b0 = 0x36;
                g_BattlePeGauges[1][0].r1 = 0x4A;
                g_BattlePeGauges[1][0].g1 = 0xFF;
                g_BattlePeGauges[1][0].b1 = 0x3B;
                g_BattlePeGauges[1][0].r2 = 0;
                g_BattlePeGauges[1][0].g2 = 0x82;
                g_BattlePeGauges[1][0].b2 = 0x36;
                g_BattlePeGauges[1][0].r3 = 0x4A;
                g_BattlePeGauges[1][0].g3 = 0xFF;
                g_BattlePeGauges[1][0].b3 = 0x3B;
            }
        }
        temp_v1_4 = *temp_s2;
        if (temp_v1_4 & 0x800000) {
            temp_v0_6 = Battle_GetReactionTimer() - 1;
            Battle_SetReactionTimer(temp_v0_6);
            if ((temp_v0_6 << 0x18) <= 0) {
                *temp_s2 &= 0xFF7FFFFF;
            }
        } else if (!(temp_v1_4 & 0x180000)) {
            var_s0 = (BattleEntity *)g_FieldActorListHead;
            if (var_s0 != NULL) {
                do {
                    if (var_s0 != D_8009CD10) {
                        {
                        register void *entity0;

                        entity0 = var_s0->core;
                        if ((entity0 != NULL) && !(var_s0->entityFlags & 0x10)) {
                            temp_v1_5 = entity0;
                            temp_a0_2 = temp_v1_5->effect;
                            if ((temp_a0_2 != NULL) && (*temp_s2 & 0x4000)) {
                                temp_v1_6 = temp_v1_5->coreFlags;
                                if ((temp_v1_6 < 0) && ((temp_a0_2->state - 1) < 2U)) {
                                    temp_v1_7 = ((unsigned int)temp_v1_6 >> 0x15) & 7;
                                    if (temp_v1_7 < 3) {
                                        Asset_Find08w(temp_v1_5->contactAssetIds[temp_v1_7], 0, var_s0->renderObject.target_x, var_s0->renderObject.target_y, var_s0->renderObject.target_z);
                                    }
                                    *temp_s2 |= 0x10000000;
                                    Entity_ResolveDropTable(var_s0);
                                    D_8009CD10->motionX = 0;
                                    D_8009CD10->motionY = 0;
                                    D_8009CD10->motionZ = 0;
                                    {
                                    u16 v9c;
                                    s32 vflags;

                                    v9c = temp_v1_5->rewardFactorB;
                                    vflags = temp_v1_5->coreFlags;
                                    v9c = (u16) (v9c + 1);
                                    vflags &= 0x7FFFFFFF;
                                    temp_v1_5->coreFlags = vflags;
                                    temp_v1_5->rewardFactorB = v9c;
                                    }
                                    {
                                    s32 s2_flags;

                                    s2_flags = *temp_s2;
                                    g_AyaStatusEffectTimer = 0x5A;
                                    *temp_s2 = s2_flags | 0x01000000;
                                    }
                                }
                                {
                                register Combatant *desc_state;

                                desc_state = g_ActiveActor;
                                temp_v1_8 = (s16) desc_state->curHP;
                                if ((s16) desc_state->maxHP >= temp_v1_8) {
                                    register BattleEntity *d254;

                                    d254 = D_8009CD10;
                                    desc_state->panelA_val = (s16) (desc_state->hpMirror - temp_v1_8);
                                    {
                                    u16 copy210;

                                    copy210 = (u16)d254->renderObject.projected_x;
                                    g_ActiveActor->panelA_x = copy210;
                                    }
                                    {
                                    u16 copy212;
                                    register Combatant *desc_tail;

                                    copy212 = (u16)d254->renderObject.projected_y;
                                    g_ActiveActor->panelA_timer = 0x1EU;
                                    desc_tail = g_ActiveActor;
                                    desc_state->panelA_y = copy212;
                                    if (desc_tail->panelA_val == 0) {
                                        desc_tail->panelA_scale = 0;
                                    } else {
                                        desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                                    }
                                    }
                                    g_ActiveActor->stateFlags = g_ActiveActor->stateFlags & 0xFFFF7FFF;
                                }
                                }
                            }
                            if (var_s0->entityFlags & 0x02000000) {
                                temp_v1_9 = *temp_s2;
                                if (!(temp_v1_9 & 0x01000000) && (temp_v1_5->hpAlive > 0)) {
                                    if (!(temp_v1_9 & 0x200)) {
                                        g_ActiveActor->curHP = (s16) (g_ActiveActor->curHP - temp_v1_5->contactDamage);
                                    }
                                    {
                                    s32 s2_flags2;
                                    register Combatant *desc_state;

                                    g_AyaStatusEffectTimer = 0x5A;
                                    s2_flags2 = *temp_s2;
                                    desc_state = g_ActiveActor;
                                    *temp_s2 = s2_flags2 | 0x01000000;
                                    temp_v1_10 = (s16) desc_state->curHP;
                                    if ((s16) desc_state->maxHP >= temp_v1_10) {
                                        register BattleEntity *d254;

                                        d254 = D_8009CD10;
                                        desc_state->panelA_val = (s16) (desc_state->hpMirror - temp_v1_10);
                                        {
                                        u16 copy210;

                                        copy210 = (u16)d254->renderObject.projected_x;
                                        desc_state->panelA_x = copy210;
                                        }
                                        {
                                        u16 copy212;
                                        register Combatant *desc_tail;

                                        copy212 = (u16)d254->renderObject.projected_y;
                                        desc_state->panelA_timer = 0x1EU;
                                        desc_tail = g_ActiveActor;
                                        desc_state->panelA_y = copy212;
                                        if (desc_tail->panelA_val == 0) {
                                            desc_tail->panelA_scale = 0;
                                        } else {
                                            desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                                        }
                                        }
                                        g_ActiveActor->stateFlags = g_ActiveActor->stateFlags & 0xFFFF7FFF;
                                    }
                                    }
                                    {
                                    register s32 clear_flags;

                                    clear_flags = 0xFFF3FFFF;
                                    D_8009CD10->motionX = 0;
                                    D_8009CD10->motionY = 0;
                                    D_8009CD10->motionZ = 0;
                                    D_8009CD10->entityFlags = D_8009CD10->entityFlags & clear_flags;
                                    g_ActiveActor->knockbackAngle = Entity_ApplyHitAndSetAnim(var_s0);
                                    }
                                    g_ActiveActor->knockbackDistance = temp_v1_5->contactKnockback;
                                    g_ActiveActor->knockbackFrames = 6;
                                }
                            }
                        }
                        }
                    }
                    var_s0 = var_s0->next;
                } while (var_s0 != NULL);
            }
            {
            register Combatant *desc_state;

            desc_state = g_ActiveActor;
            temp_v1_11 = (s16) desc_state->curHP;
            temp_v0_7 = (s16) desc_state->hpMirror;
            if (temp_v1_11 < temp_v0_7) {
                if ((s16) desc_state->maxHP >= temp_v1_11) {
                    register BattleEntity *d254;

                    d254 = D_8009CD10;
                    desc_state->panelA_val = (s16) (temp_v0_7 - temp_v1_11);
                    {
                    u16 copy210;

                    copy210 = (u16)d254->renderObject.projected_x;
                    g_ActiveActor->panelA_x = copy210;
                    }
                    {
                    u16 copy212;
                    register Combatant *desc_tail;

                    copy212 = (u16)d254->renderObject.projected_y;
                    g_ActiveActor->panelA_timer = 0x1EU;
                    desc_tail = g_ActiveActor;
                    desc_state->panelA_y = copy212;
                    if (desc_tail->panelA_val == 0) {
                        desc_tail->panelA_scale = 0;
                    } else {
                        desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                    }
                    }
                    g_ActiveActor->stateFlags = g_ActiveActor->stateFlags & 0xFFFF7FFF;
                }
                g_ActiveActor->hpMirror = g_ActiveActor->curHP;
            }
            }
        }
        *temp_s2 &= ~0x4000;
        if (g_ActiveActor->knockbackFrames != 0) {
            temp_a0_3 = D_8009CD10->entityFlags;
            if ((temp_a0_3 & 0xC0000) || (g_ActiveActor->knockbackDistance == 0)) {
                D_8009CD10->entityFlags = temp_a0_3 & 0xFFF3FFFF;
                g_ActiveActor->knockbackFrames = 0;
                g_ActiveActor->knockbackDistance = 0U;
            } else {
                D_8009CD10->posX.fixed = D_8009CD10->baseX + (g_ActiveActor->knockbackDistance * rsin(g_ActiveActor->knockbackAngle) * 0x10);
                D_8009CD10->posZ.fixed = D_8009CD10->baseZ + (g_ActiveActor->knockbackDistance * rcos(g_ActiveActor->knockbackAngle) * 0x10);
                temp_v1_12 = g_ActiveActor->knockbackDistance;
                g_ActiveActor->knockbackDistance = (u8) (temp_v1_12 - (temp_v1_12 / g_ActiveActor->knockbackFrames));
                g_ActiveActor->knockbackFrames = (s8) ((u8) g_ActiveActor->knockbackFrames - 1);
            }
        }
        Entity_UpdateTimers();
    }
    if (g_ActiveActor->panelA_timer != 0) {
        Battle_DrawStatusPanel(0,
                               (struct BattleStatusPanel *)&g_ActiveActor->panelA_val);
        g_ActiveActor->panelA_timer = (u8) (g_ActiveActor->panelA_timer - 1);
    }
    {
    state0 = g_ActiveActor;
    temp_v0_8 = (s16) state0->hpMirror;
    temp_v1_13 = (s16) state0->curHP;
    if (temp_v0_8 < temp_v1_13) {
        register BattleEntity *d254;

        d254 = D_8009CD10;
        state0->panelB_val = (s16) (temp_v1_13 - temp_v0_8);
        {
        u16 copy210;

        copy210 = (u16)d254->renderObject.projected_x;
        g_ActiveActor->panelB_x = copy210;
        }
        {
        var_v0.value = (u16)d254->renderObject.projected_y;
        var_v1 = 0x1E;
        g_ActiveActor->panelB_timer = var_v1;
        { register Combatant *post_state; post_state = g_ActiveActor;
        state0->panelB_y = (s16) (var_v0.value - 8);
        post_state->panelB_flag = 1; }
        }
        g_ActiveActor->hpMirror = g_ActiveActor->curHP;
    }
    }
    temp_a1_2 = g_ActiveActor;
    if (temp_a1_2->panelB_timer != 0) {
        Battle_DrawStatusPanel(0, (struct BattleStatusPanel *)&temp_a1_2->panelB_val);
        g_ActiveActor->panelB_timer = (u8) (g_ActiveActor->panelB_timer - 1);
        temp_a1_2 = g_ActiveActor;
    }
    if (temp_a1_2->panelAux_timer != 0) {
        Battle_DrawStatusPanel(0, (struct BattleStatusPanel *)&temp_a1_2->panelAux_val);
        g_ActiveActor->panelAux_timer = (u8) (g_ActiveActor->panelAux_timer - 1);
    }
    temp_a0_4 = g_ActiveActor->exp_or_acc;
    g_ActiveActor->hpMirror = g_ActiveActor->curHP;
    if (temp_a0_4 <= 0) {
        *temp_s2 |= 0x2000;
        if (Pad_GetMenuPressedBitOrDisabled() != g_BattleActionIndex) {
            if (g_BattleAttackPmSlot != -1) {
                Pm_SendCmd(g_BattleAttackPmSlot, 0, 0, 2, 0, 0);
                g_BattleAttackPmSlot = -1;
            }
            if (g_BattleImpactPmSlot != -1) {
                Pm_SendCmd(g_BattleImpactPmSlot, 0, 0, 2, 0, 0);
                g_BattleImpactPmSlot = -1;
            }
        }
        Battle_FlushScriptSounds();
    }
    if (g_BattleModeState == 1) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v0.value = 0xFF;
        var_v1 = 0xF9;
        g_BattleAtbGauges[0].g0 = var_a2;
        g_BattleAtbGauges[0].g2 = var_a2;
        g_BattleAtbGauges[1].g0 = var_a2;
        g_BattleAtbGauges[1].g2 = var_a2;
        var_a2 = 0x36;
        g_BattleAtbGauges[0].r1 = var_a0;
        g_BattleAtbGauges[0].r3 = var_a0;
        g_BattleAtbMarkers[0].sprite.color.bytes.r = var_a0;
        g_BattleAtbGauges[1].r1 = var_a0;
        g_BattleAtbGauges[1].r3 = var_a0;
        g_BattleAtbMarkers[1].sprite.color.bytes.r = var_a0;
        var_a0 = 0x4A;
        g_BattleAtbGauges[0].b1 = var_v1;
        g_BattleAtbGauges[0].b3 = var_v1;
        g_BattleAtbMarkers[0].sprite.color.bytes.b = var_v1;
        g_BattleAtbGauges[1].b1 = var_v1;
        g_BattleAtbGauges[1].b3 = var_v1;
        g_BattleAtbMarkers[1].sprite.color.bytes.b = var_v1;
        var_v1 = 0x3B;
        g_BattleAtbGauges[0].r0 = 0;
        g_BattleAtbGauges[0].b0 = var_a1;
        g_BattleAtbGauges[0].g1 = var_v0.value;
        g_BattleAtbGauges[0].r2 = 0;
        g_BattleAtbGauges[0].b2 = var_a1;
        g_BattleAtbGauges[0].g3 = var_v0.value;
        g_BattleAtbMarkers[0].sprite.color.bytes.g = var_v0.value;
        g_BattleAtbGauges[1].r0 = 0;
        g_BattleAtbGauges[1].b0 = var_a1;
        g_BattleAtbGauges[1].g1 = var_v0.value;
        g_BattleAtbGauges[1].r2 = 0;
        g_BattleAtbGauges[1].b2 = var_a1;
        g_BattleAtbGauges[1].g3 = var_v0.value;
        g_BattleAtbMarkers[1].sprite.color.bytes.g = var_v0.value;
        g_BattlePeGauges[0][0].r0 = 0;
        g_BattlePeGauges[0][0].g0 = var_a1;
        g_BattlePeGauges[0][0].b0 = var_a2;
        g_BattlePeGauges[0][0].r1 = var_a0;
        g_BattlePeGauges[0][0].g1 = var_v0.value;
        g_BattlePeGauges[0][0].b1 = var_v1;
        g_BattlePeGauges[0][0].r2 = 0;
        g_BattlePeGauges[0][0].g2 = var_a1;
        g_BattlePeGauges[0][0].b2 = var_a2;
        g_BattlePeGauges[0][0].r3 = var_a0;
        g_BattlePeGauges[0][0].g3 = var_v0.value;
        g_BattlePeGauges[0][0].b3 = var_v1;
        g_BattlePeGauges[1][0].r0 = 0;
        g_BattlePeGauges[1][0].g0 = var_a1;
        g_BattlePeGauges[1][0].b0 = var_a2;
        g_BattlePeGauges[1][0].r1 = var_a0;
        g_BattlePeGauges[1][0].g1 = var_v0.value;
        g_BattlePeGauges[1][0].b1 = var_v1;
        g_BattlePeGauges[1][0].r2 = 0;
        g_BattlePeGauges[1][0].g2 = var_a1;
        g_BattlePeGauges[1][0].b2 = var_a2;
        g_BattlePeGauges[1][0].r3 = var_a0;
        g_BattlePeGauges[1][0].g3 = var_v0.value;
        g_BattlePeGauges[1][0].b3 = var_v1;
    }
    if ((s16) g_ActiveActor->curHP <= 0) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v1 = 0xF9;
        var_a3 = 0x3D;
        {
        var_v0.entity = D_8009CD10;
        var_v0.entity->motionX = 0;
        var_v0.entity->motionY = 0;
        var_v0.entity->motionZ = 0;
        }
        var_v0.value = 0xFF;
        g_BattleAtbGauges[0].g0 = var_a2;
        g_BattleAtbGauges[0].g2 = var_a2;
        g_BattleAtbGauges[1].g0 = var_a2;
        g_BattleAtbGauges[1].g2 = var_a2;
        g_BattleAtbGauges[0].r1 = var_a0;
        g_BattleAtbGauges[0].r3 = var_a0;
        g_BattleAtbMarkers[0].sprite.color.bytes.r = var_a0;
        g_BattleAtbGauges[1].r1 = var_a0;
        g_BattleAtbGauges[1].r3 = var_a0;
        g_BattleAtbMarkers[1].sprite.color.bytes.r = var_a0;
        g_BattleAtbGauges[0].r0 = 0;
        g_BattleAtbGauges[0].b0 = var_a1;
        g_BattleAtbGauges[0].g1 = var_v0.value;
        g_BattleAtbGauges[0].b1 = var_v1;
        g_BattleAtbGauges[0].r2 = 0;
        g_BattleAtbGauges[0].b2 = var_a1;
        g_BattleAtbGauges[0].g3 = var_v0.value;
        g_BattleAtbGauges[0].b3 = var_v1;
        g_BattleAtbMarkers[0].sprite.color.bytes.g = var_v0.value;
        g_BattleAtbMarkers[0].sprite.color.bytes.b = var_v1;
        g_BattleAtbGauges[1].r0 = 0;
        g_BattleAtbGauges[1].b0 = var_a1;
        g_BattleAtbGauges[1].g1 = var_v0.value;
        g_BattleAtbGauges[1].b1 = var_v1;
        g_BattleAtbGauges[1].r2 = 0;
        g_BattleAtbGauges[1].b2 = var_a1;
        g_BattleAtbGauges[1].g3 = var_v0.value;
        g_BattleAtbGauges[1].b3 = var_v1;
        g_BattleAtbMarkers[1].sprite.color.bytes.g = var_v0.value;
        g_BattleAtbMarkers[1].sprite.color.bytes.b = var_v1;
        {
        var_a2 = 0x36;
        var_a0 = 0x4A;
        var_v1 = 0x3B;
        g_BattlePeGauges[0][0].b0 = var_a2;
        g_BattlePeGauges[0][0].b2 = var_a2;
        g_BattlePeGauges[1][0].b0 = var_a2;
        g_BattlePeGauges[1][0].b2 = var_a2;
        var_a2 = 0x81;
        g_BattlePeGauges[0][0].g0 = var_a1;
        g_BattlePeGauges[0][0].g2 = var_a1;
        g_BattlePeGauges[1][0].g0 = var_a1;
        g_BattlePeGauges[1][0].g2 = var_a1;
        var_a1 = 0x83;
        g_BattlePeGauges[0][0].r1 = var_a0;
        g_BattlePeGauges[0][0].r3 = var_a0;
        g_BattlePeGauges[1][0].r1 = var_a0;
        g_BattlePeGauges[1][0].r3 = var_a0;
        var_a0 = 0x13;
        }
        g_BattlePeGauges[0][0].b1 = var_v1;
        g_BattlePeGauges[0][0].b3 = var_v1;
        g_BattlePeGauges[1][0].b1 = var_v1;
        g_BattlePeGauges[1][0].b3 = var_v1;
        var_v1 = 1;
        g_BattlePeGauges[0][0].r0 = 0;
        g_BattlePeGauges[0][0].g1 = var_v0.value;
        g_BattlePeGauges[0][0].r2 = 0;
        g_BattlePeGauges[0][0].g3 = var_v0.value;
        g_BattlePeGauges[1][0].r0 = 0;
        g_BattlePeGauges[1][0].g1 = var_v0.value;
        g_BattlePeGauges[1][0].r2 = 0;
        g_BattlePeGauges[1][0].g3 = var_v0.value;
        {
        g_BattlePeGauges[0][1].r0 = var_v0.value;
        g_BattlePeGauges[0][1].g0 = var_a3;
        g_BattlePeGauges[0][1].b0 = var_a2;
        g_BattlePeGauges[0][1].r1 = var_a1;
        g_BattlePeGauges[0][1].g1 = var_a0;
        g_BattlePeGauges[0][1].b1 = var_v1;
        g_BattlePeGauges[0][1].r2 = var_v0.value;
        g_BattlePeGauges[0][1].g2 = var_a3;
        g_BattlePeGauges[0][1].b2 = var_a2;
        g_BattlePeGauges[0][1].r3 = var_a1;
        g_BattlePeGauges[0][1].g3 = var_a0;
        g_BattlePeGauges[0][1].b3 = var_v1;
        g_BattlePeGauges[1][1].r0 = var_v0.value;
        g_BattlePeGauges[1][1].g0 = var_a3;
        g_BattlePeGauges[1][1].b0 = var_a2;
        g_BattlePeGauges[1][1].r1 = var_a1;
        g_BattlePeGauges[1][1].g1 = var_a0;
        g_BattlePeGauges[1][1].b1 = var_v1;
        g_BattlePeGauges[1][1].r2 = var_v0.value;
        g_BattlePeGauges[1][1].g2 = var_a3;
        g_BattlePeGauges[1][1].b2 = var_a2;
        g_BattlePeGauges[1][1].r3 = var_a1;
        g_BattlePeGauges[1][1].g3 = var_a0;
        g_BattlePeGauges[1][1].b3 = var_v1;
        }
        if (*temp_s2 & 0x10000) {
            Battle_ReturnToIdle();
        }
        Draw_ResetTextboxes();
        if (Pad_GetMenuPressedBitOrDisabled() != g_BattleActionIndex) {
            if (g_BattleAttackPmSlot != -1) {
                Pm_SendCmd(g_BattleAttackPmSlot, 0, 0, 2, 0, 0);
                g_BattleAttackPmSlot = -1;
            }
            if (g_BattleImpactPmSlot != -1) {
                Pm_SendCmd(g_BattleImpactPmSlot, 0, 0, 2, 0, 0);
                g_BattleImpactPmSlot = -1;
            }
        }
        Battle_FlushScriptSounds();
        {
        register s32 tail_mask;
        register s32 tail_d1ac;
        register s32 tail_a0;

        tail_a0 = 0x46B;
        tail_mask = ~0x300;
        tail_d1ac = g_BattleResultFlags;
        tail_d1ac &= tail_mask;
        g_BattleResultFlags = tail_d1ac;
        {
            register s32 tail_a1;
            register s32 tail_a2;
            register s32 tail_a3;
            register s32 tail_sp;

            s16 *coordinate;

            var_t0.entity = D_8009CD10;
            /* posX/posY/posZ integer halves, read through a plain halfword
             * pointer: struct member reads let the scheduler hoist the
             * g_BattleModeState store above them. */
            coordinate = &var_t0.entity->posX.parts.integer;
            tail_a2 = *coordinate;
            coordinate = &var_t0.entity->posY.parts.integer;
            tail_a3 = *coordinate;
            g_BattleModeState = 3;
            coordinate = &var_t0.entity->posZ.parts.integer;
            tail_sp = *coordinate;
            tail_a1 = 0;
            g_BattleSaveOverlayActive = 0;
            Asset_Find08Alt(tail_a0, tail_a1, tail_a2, tail_a3, tail_sp);
        }
        }
        g_BattleEnemyEscapeFlag = 0;
        MenuWidget_InitPool();
        if (g_GeomScroll.state.flags & 0x2000) {
            Render_BeginSceneLoad();
        }
        g_GameStateFlags &= ~4;
        D_8009CDA8 |= 1;
        Entity_SetActionMode(D_8009CD10, 0x13);
            g_ActiveActor->curHP = 0;
    }
}
