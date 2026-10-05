/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern struct { Combatant *value; char pad[12]; } D_8009D278_absolute __asm__("D_8009D278");
extern struct { BattleEntity *value; char pad[12]; } D_8009D20C_absolute __asm__("D_8009D20C");
extern struct { BattleEntity *value; char pad[12]; } D_8009D254_absolute __asm__("D_8009D254");
extern struct { u8 value; char pad[15]; } D_8009D1D4_absolute __asm__("D_8009D1D4");
extern struct { u8 value; char pad[15]; } D_8009D23C_read __asm__("D_8009D23C");
extern struct { u8 value; char pad[15]; } D_8009D23C_write __asm__("D_8009D23C");
extern struct { s32 value; char pad[12]; } D_8009D208_absolute __asm__("D_8009D208");
extern u8 D_8009CE68;
extern BattleEntity *D_800B8A90[];
#define INDEX (D_8009D1D4_absolute.value)
#define QUEUE_COUNT_READ (D_8009D23C_read.value)
#define QUEUE_COUNT_WRITE (D_8009D23C_write.value)
#define LOAD_RESULT (D_8009D208_absolute.value)
int Battle_ProcessActionSlot(BattleEntity *entity)
{
    register BattleEntity *actor asm("$7") = entity;
    /* Fix the entity pointer in a3 before the prologue saves registers. */
    asm volatile("" : : "r"(actor));
    {
        register EnemyCombatant *state asm("$17") = (EnemyCombatant *)actor->core;
        int phase = state->coreFlags & 0x6000;
        int result = 1;

        if (phase == 0x2000) {
            int index = INDEX;
            s16 command = D_800BE830[index].field04;
            if (command != 0x196 && command != 0x189) {
                if (D_8009D278_absolute.value->action->actionCode.actionId != 6) {
                    if (QUEUE_COUNT_READ == 0 && D_800B8A90[0] != 0) {
                        int i = 0;
                        do {
                            D_800B8A90[(u8)i] = 0;
                            i++;
                        } while (D_800B8A90[(u8)i] != 0);
                    }
                    {
                        u8 count = QUEUE_COUNT_READ;
                        QUEUE_COUNT_WRITE = count + 1;
                        D_800B8A90[count] = actor;
                    }
                } else {
                    register int room asm("$4") = 0x5B;
                    if (actor != D_800BE830[index].actor) goto after_room;
                    LOAD_RESULT = Scene_LoadRoomAssets(room, actor);
                    QUEUE_COUNT_WRITE = 0;
                after_room:;
                }
            }
            result = 0;
            if ((s8)state->field04.bytes.field05 == 0 || state->hpAlive <= 0) goto done;
            D_8009CE68 = 0xFF;
            goto done;
        }
        if (phase != 0x4000 || (s8)state->field04.bytes.field05 == 0) goto done;

        {
            register int one asm("$2") = 1;
            u8 color = D_8009CE68 - 8;
            D_8009CE68 = color;
            if ((s8)state->field04.bytes.field05 == one) {
                if (color <= 0x80) {
                    actor->parent->renderObject.flags_9C |= 0x20;
                    goto clear_state;
                } else {
                    Render_FadeEntityColor(&actor->parent->renderObject, color, color, color);
                    result = 0;
                    goto done;
                }
            }
            if ((s8)state->field04.bytes.field05 == 4) {
                BattleEntity *iter = D_8009D20C_absolute.value;
                for (; iter != 0; iter = iter->next) {
                    if (iter != D_8009D254_absolute.value && iter->core != 0 &&
                        (s8)((EnemyCombatant *)iter->core)->field04.bytes.field05 == 4) {
                        int shade = D_8009CE68;
                        if ((u8)shade <= 0x80) {
                            iter->renderObject.flags_9C |= 0x20;
                            state->coreFlags &= ~0x6000;
                        } else {
                            Render_FadeEntityColor(&iter->renderObject, shade, shade, shade);
                            result = 0;
                        }
                        /* Preserve the core flag reload across iterations. */
                        asm volatile("" : : : "memory");
                    }
                }
                goto done;
            }
            if (state->hpAlive <= 0) goto done;
            if (color <= 0x80) {
                actor->renderObject.flags_9C |= 0x20;
            clear_state:
                state->coreFlags &= ~0x6000;
                goto done;
            }
            {
                register int green asm("$6") = color;
                /* Keep the green channel in a2 at the call boundary. */
                asm volatile("" : "=r"(green) : "0"(green));
                Render_FadeEntityColor(&actor->renderObject, color, green, color);
                result = 0;
            }
        }
    done:
        return result;
    }
}
/* Advance enemy charge/status timers, movement, damage panels and death state.
* Matching debt: 10 register pins and 3 empty compiler barriers retain retail
* register lifetimes and the distinct core/statusCore aliases. No instruction ASM.
* curHP is the existing name for the 0..9000 charge gauge in this core view.
*/
void Battle_UpdateEnemy(BattleEntity *entity) {
    register BattleEntity *actor asm("$16") = entity;
    register EnemyCombatant *statusCore asm("$18");
    BattleEntity *parent;
    register BattleEntity *livingLink asm("$5");
    BattleEntity *linkedActor;
    EnemyCombatant *linkedCore;
    EnemyCombatant *enemy;
    EnemyCombatant *otherCore;
    s16 angle;
    s32 hpChange;
    s32 actionPhase;
    s32 damageTicks;
    s32 advancedAngle;
    s32 wrappedAngle;
    EnemyActionEffect *expiredEffect;
    EnemyActionEffect *expiredStatusEffect;
    s8 linkedKindValue;
    s8 effectKind;
    s8 deadKind;
    u16 nextCharge;
    u16 charge;
    u32 initialFlags;
    u32 statusFlags;
    u32 damageFlags;
    register u32 nextFlags asm("$3");
    u32 nextStatusFlags;
    u32 actionFlags;
    register u32 resumePhase asm("$4");

    asm("" : "=r"(actor) : "0"(actor));
    enemy = actor->core;
    {
        s32 hp = enemy->hpAlive;
        s32 maximum = enemy->field88;
        if (maximum < hp) enemy->hpAlive = maximum;
        statusCore = enemy;
        asm("" : "=r"(statusCore) : "0"(statusCore));
    }
    if (!(g_BattleGameStateWindow.flags & 0x100)) {
        if (enemy->curHP == 0) {
            initialFlags = enemy->coreFlags;
            if (initialFlags & 0xE) {
                register u32 preserved asm("$3") = initialFlags & ~0xE;
                register u32 count asm("$2") = ((((initialFlags >> 1) & 7) - 1) & 7) * 2;
                nextFlags = preserved | count;
                enemy->coreFlags = nextFlags;
                if (!(nextFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredEffect = enemy->effect;
                    if (expiredEffect != 0) {
                        expiredEffect->state = 4;
                        enemy->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
            statusFlags = statusCore->coreFlags;
            if (statusFlags & 0x1800) {
                nextStatusFlags = (statusFlags & ~0x1800) | (((((statusFlags >> 0xB) & 3) - 1) & 3) << 0xB);
                statusCore->coreFlags = nextStatusFlags;
                if (!(nextStatusFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredStatusEffect = enemy->effect;
                    if (expiredStatusEffect != 0) {
                        expiredStatusEffect->state = 4;
                        statusCore->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
        }
        charge = enemy->curHP;
        if (charge < 0x2328U) {
            nextCharge = charge + enemy->chargeStep;
            enemy->curHP = nextCharge;
            if (statusCore->coreFlags & 1) {
                enemy->curHP = nextCharge - ((enemy->chargeStep * 2) / 5);
            }
        } else {
            enemy->curHP = 0x2328;
            if (actor->entityFlags & 0x1000) {
                enemy->curHP = 0;
            }
        }
        damageFlags = statusCore->coreFlags;
        if (damageFlags & 0x10) {
            damageTicks = (damageFlags >> 5) & 0x1F;
            if (damageTicks >= 0x1E) {
                enemy->hpAlive -= enemy->periodicDamage;
                statusCore->coreFlags &= ~0x3E0;
            } else {
                statusCore->coreFlags = (damageFlags & ~0x3E0) | (((damageTicks + 1) & 0x1F) << 5);
            }
        }
    }
    if (statusCore->coreFlags & 0x6000) {
        Battle_ProcessActionSlot(actor);
        actionFlags = statusCore->coreFlags;
        actionPhase = actionFlags & 0x6000;
        if (actionPhase == 0x2000) {
            Battle_StepEnemyMovement(actor);
            Asset_Find08w((s32) enemy->attackAssetId, 0, (s32) actor->renderObject.target_x, (s32) actor->renderObject.target_y, (s32) actor->renderObject.target_z);
        } else if (actionPhase == 0x4000) {
            s32 kind = (s8)enemy->field04.bytes.field05;
            register u32 actionBits asm("$2");
            if (kind == 0) {
                actionBits = actionFlags & 0xE;
                if (actionBits == 0) {
                    if (actor->animLastFrame == actor->animPrev.parts.integer) {
                        register int resumeKind asm("$2") = 2;
                        resumePhase = enemy->resumePhase;
                        asm("" : "=r"(resumePhase) : "0"(resumePhase), "r"(resumeKind) : "memory");
                        if (((resumePhase & 0xFF) == resumeKind) && !(enemy->coreFlags & 0x1800)) {
                            enemy->resumePhase = resumePhase - 1;
                            Entity_SetActionMode(actor, (s32) enemy->savedActionMode);
                            if (enemy->savedInterpolation != 0) {
                                actor->entityFlags |= 0x200;
                            }
                            actor->animFrame = enemy->savedAnimFrame;
                            actor->animPrev.fixed = enemy->savedAnimPrev;
                            actor->animStep = enemy->savedAnimStep;
                        } else {
                            {
                                register BattleEntity *callEntity asm("$4");
                                unsigned mode = enemy->field06.bytes.low;
                                callEntity = actor;

                                Entity_SetActionMode(callEntity, (u16)(s8)mode);
                            }
                        }
                        statusCore->coreFlags &= ~0x6000;
                        if (!(D_8009D278->stateFlags & 0x80000) && !(enemy->coreFlags & 0x1800)) {
                            actor->entityFlags &= ~0x1000;
                            Entity_TickAnimSequences(actor);
                        }
                    } else if (((u8) enemy->motionPhase >= 2U) && (enemy->motionFrames != 0)) {
                        actor->posX.fixed += enemy->motionAmplitude * rsin((s32) enemy->motionAngle) * 0x10;
                        actor->posZ.fixed += enemy->motionAmplitude * rcos((s32) enemy->motionAngle) * 0x10;
                        enemy->motionFrames -= 1;
                    }
                } else {
                    goto finishInterruptedAction;
                }
            } else {
                actionBits = actionFlags & 0xE;
                if (actionBits != 0) {
finishInterruptedAction:
                    statusCore->coreFlags &= ~0x6000;
                    if (((s8) enemy->field04.bytes.field05 != 0) && (enemy->hpAlive > 0)) {
                        effectKind = (s8) ((EnemyCombatant *)actor->core)->field04.bytes.field05;
                        if (effectKind == 1) {
                            parent = actor->parent;
                            parent->renderObject.flags_9C |= 0x20;
                        } else if (effectKind == 4) {
                            int linkedKind = 4;
                            BattleEntity *player;
                            linkedActor = D_8009D20C;
                            if (linkedActor != 0) {
                                player = D_8009D254;
updateLinkedActor:
                                if (linkedActor != player) {
                                    otherCore = linkedActor->core;
                                    if ((otherCore != 0) && ((s8) otherCore->field04.bytes.field05 == linkedKind)) {
                                        linkedActor->renderObject.flags_9C |= 0x20;
                                    }
                                }
                                linkedActor = linkedActor->next;
                                if (linkedActor != 0) {
                                    goto updateLinkedActor;
                                }
                            }
                        } else {
                            actor->renderObject.flags_9C |= 0x20;
                        }
                    }
                }
            }
        }
        if ((s8) enemy->field04.bytes.field05 != 1) {
            actor->motionX = 0;
            actor->motionY = 0;
            actor->motionZ = 0;
        }
    }
    if ((statusCore->coreFlags & 0xE) && (enemy->hpAlive > 0)) {
        int contextFlags = g_BattleGameStateWindow.flags;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        if (!(contextFlags & 0x100)) {
            angle = actor->facingAngle;
            advancedAngle = angle + 0x80;
            wrappedAngle = advancedAngle;
            if (advancedAngle < 0) {
                wrappedAngle = angle + 0x107F;
            }
            actor->facingAngle = advancedAngle - ((wrappedAngle >> 0xC) << 0xC);
        }
    }
    if (statusCore->coreFlags & 0x1800) {
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
    }
    if (statusCore->coreFlags & 0x400) {
        enemy->hpAlive = -1;
    }
    hpChange = enemy->previousHP - enemy->hpAlive;
    if (hpChange != 0) {
        if (hpChange < 0) {
            enemy->panelC_val = -hpChange;
            enemy->panelC_mode = 1;
        } else {
            enemy->panelC_val = hpChange;
            if ((statusCore->coreFlags & 0x38000) == 0x18000) {
                enemy->panelC_mode = 2;
            } else {
                enemy->panelC_mode = 0;
            }
        }
        enemy->panelC_x = (u16) actor->renderObject.projected_target_x;

        enemy->panelC_y = (u16) actor->renderObject.projected_target_y - 0x14;
        enemy->panelC_timer = 0x1E;
    }
    if (enemy->panelC_timer != 0) {
        Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
        enemy->panelC_timer -= 1;
    }
    if (enemy->hpAlive <= 0) {
        if (!(g_BattleGameStateWindow.flags & 0x100)) {
            if (((s8) enemy->field04.bytes.field05 == 1) && !(enemy->coreFlags & 0x6000)) {
                actor->entityFlags |= 0x10;
                Battle_SlotFree(actor);
            }
            if ((s8) enemy->field04.bytes.field05 == 4) {
                if (enemy->coreFlags & 0x6000) goto saveHpSnapshot;
                goto stepDeathAnimation;
            } else if ((s8) enemy->field04.bytes.field05 != 1 && (s8) enemy->field04.bytes.field05 != 3) {
stepDeathAnimation:
                actor->motionX = 0;
                actor->motionY = 0;
                actor->motionZ = 0;
                Battle_StepEntityAnimState(actor);
            } else {
                livingLink = D_8009D20C;
findLivingLink:
                if (livingLink == 0) {
                    deadKind = (s8) enemy->field04.bytes.field05;
                    if (deadKind != 3) {
                        if (deadKind == 1) {
                            actor->entityFlags |= 0x10;
                            Battle_SlotFree(actor);
                            actor->parent->motionX = 0;
                            actor->parent->motionY = 0;
                            actor->parent->motionZ = 0;
                            Battle_StepEntityAnimState(actor->parent);
                        }
                    } else {
                        goto stepDeathAnimation;
                    }
                } else {
                    linkedCore = livingLink->core;
                    if ((linkedCore == 0) || (livingLink == D_8009D254) || (linkedKindValue = (s8) linkedCore->field04.bytes.field05, (linkedKindValue == 0)) || (linkedKindValue == 2) || (linkedKindValue == 4) || (linkedCore->hpAlive <= 0)) {
                        livingLink = livingLink->next;
                        goto findLivingLink;
                    }
                }
            }
        }
    }
saveHpSnapshot:
    enemy->previousHP = enemy->hpAlive;
}
