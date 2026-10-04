/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

void Akao_Cmd_C1_WithSlot(int slot, int duration, int volume);
void Akao_Cmd_21(int slot, int value);
int Render_StartFadeIn(int);
void Sys_Shutdown(void);

/* COMMON metadata preserves GP-relative accesses; retail symbols supply storage. */
u8 D_8009CE70, D_8009CE74;
s32 D_8009D28C;

/* End-of-battle sequence: retire enemy actors, fade sound and animate the
 * textured panel before completing the battle phase. Matching debt: 16 register pins,
 * 7 empty barriers, two volatile packet views and 24 reserved stack bytes.
 * Shared switch tails retain the original countdown and stage-store layout. */
void Battle_StepPostBattle(void)
{
    volatile u8 matchingStackReserve[24];
    BattleEntity *player;
    BattleEntity *entity;
    BattleEntity *fadingEntity;
    register BattleEntity *hiddenEntity asm("$16");
    RenderTexturedQuad *expandingPanel;
    volatile RenderTexturedQuad *fadingPanel;
    register volatile RenderTexturedQuad *raisingPanel asm("$2");
    s32 remainingFrames;
    register s32 horizontalInset asm("$7");
    s8 brightness;
    register u16 leftEdge asm("$4");
    u16 rightEdge;
    u32 entityFlags;
    u8 fadeBrightness;
    register u8 countdown asm("$2");
    u8 nextStage;
    EnemyCombatant *core;
    EnemyCombatant *actionCore;
    EnemyCombatant *fadingCore;

    switch (D_8009CE74) {
    case 0:
        player = D_8009D254;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009D254;
        }
        if (player->animLastFrame == (u16)(player->animFrame >> 16)) {
            entity = D_8009D20C;
            if (entity != 0) {
                do {
                    if (entity != D_8009D254) {
                        core = entity->core;
                        if (((s8)core->field04.bytes.field05 != 1) && ((core != 0) || !(entity->entityFlags & 0x40))) {
                            entity->renderObject.flags_9C |= 2;
                            actionCore = entity->core;
                            entity->entityFlags |= 0x1000;
                            if (actionCore != 0) {
                                {
                                    u8 mode = actionCore->field06.bytes.low;
                                    asm("" : "=r"(mode) : "0"(mode));
                                    Entity_SetActionMode(entity, (s8)mode & 0xFFFF);
                                }
                                entityFlags = entity->entityFlags;
                                if ((entityFlags & 0x40000000) && (((EnemyCombatant *)entity->core)->deathPersist == 0)) {
                                    entity->entityFlags = entityFlags | 0x10;
                                    entity->core = 0;
                                }
                            }
                            entity->motionX = 0;
                            entity->motionY = 0;
                            entity->motionZ = 0;
                        }
                    }
                    entity = entity->next;
                } while (entity != 0);
            }
            Battle_ResetEnemyStats(0);
            D_8009CE70 = 0x46;
            D_8009CE74 += 1;
            D_8009D254->entityFlags |= 0x100;
        }
        else {
            player->entityFlags &= ~0x100;
        }
        break;
    case 1: {
        s32 exemptKind;
        if (D_8009CE70 == 0x3C) {
            Render_StartFadeIn(0x3C);
            Akao_Cmd_C1_WithSlot(0, 0x3C, 0);
            exemptKind = 1;
            fadingEntity = D_8009D20C;
            if (fadingEntity != 0) {
                fadeNextEntity:
                if (fadingEntity != D_8009D254) {
                    fadingCore = fadingEntity->core;
                    if (((s8)fadingCore->field04.bytes.field05 != exemptKind) && ((fadingCore != 0) || !(fadingEntity->entityFlags & 0x40))) {
                        Anim_SetInterpRate(&fadingEntity->renderObject, 0x3C);
                    }
                }
                fadingEntity = fadingEntity->next;
                if (fadingEntity != 0) {
                    goto fadeNextEntity;
                }
            }
        }
        else {
            hiddenEntity = D_8009D20C;
            if (hiddenEntity != 0) {
                do {
                    if ((hiddenEntity != D_8009D254) && ((hiddenEntity->core != 0) || !(hiddenEntity->entityFlags & 0x40)) && (hiddenEntity->renderObject.variant_visible == 0)) {
                        hiddenEntity->entityFlags |= 0x10;
                    }
                    hiddenEntity = hiddenEntity->next;
                } while (hiddenEntity != 0);
            }
        }
        countdown = D_8009CE70;
        if (countdown == 0) {
            Pm_StopAllBoth();
            Akao_Cmd_21(0, 0xFF);
            {
                u8 state = D_8009CE74;
                D_8009CE70 = 0x1E;
                asm volatile("" : "=r"(state) : "0"(state) : "memory");
                nextStage = state + 1;
            }
            goto storeNextStage;
        }
        else {
            countdown--;
            goto storeCountdown;
        }
        break;
    }
    case 2: {
        register s32 brightnessBase asm("$4") = 120;
        register u16 top asm("$5") = 122;
        u8 timer;
        register RenderTexturedQuad *base asm("$3");
        s32 frame;
        asm volatile("" : : "r"(brightnessBase), "r"(top) : "memory");
        brightness = brightnessBase - (D_8009CE70 * 4);
        *(D_800BE9F4 + (D_8009CDDC * 0x28)) = brightness;
        *(D_800BE9F5 + (D_8009CDDC * 0x28)) = brightness;
        *(D_800BE9F6 + (D_8009CDDC * 0x28)) = brightness;
        base = D_800BE9F0;
        frame = D_8009CDDC;
        expandingPanel = &base[frame];
        timer = D_8009CE70;
        asm volatile("" : "=r"(expandingPanel) : "0"(expandingPanel), "r"(timer));
        remainingFrames = timer & 0xFF;
        horizontalInset = remainingFrames * 4;
        leftEdge = 0x64 - horizontalInset;
        rightEdge = ((remainingFrames * 8) + 0xDC) - horizontalInset;
        expandingPanel->x0 = leftEdge;
        expandingPanel->x2 = leftEdge;
        expandingPanel->y0 = top;
        expandingPanel->x1 = rightEdge;
        expandingPanel->y1 = top;
        expandingPanel->y2 = 0x7C;
        expandingPanel->x3 = rightEdge;
        expandingPanel->y3 = 0x7C;
        if (remainingFrames != 0) {
            {
                u8 nextTimer = timer - 1;
                D_8009CE70 = nextTimer;
            }
        }
        else {
            {
                u8 state = D_8009CE74;
                D_8009CE70 = 0x50;
                asm volatile("" : "=r"(state) : "0"(state) : "memory");
                nextStage = state + 1;
            }
            goto storeNextStage;
        }
        break;
    }
    case 3: {
        register u16 bottom asm("$3");
        if ((u8) D_8009CE70 >= 0x1AU) {
            u32 pulse = (u32)(rcos((D_8009CE70 - 16) * 16) * 11) >> 11;
            register RenderTexturedQuad *base asm("$5") = D_800BE9F0;
            register u16 left asm("$6") = 100;
            s32 frame = D_8009CDDC;
            u16 top;
            register u16 right asm("$5");
            pulse &= 255;
            raisingPanel = &base[frame];
            asm volatile("" : "=r"(raisingPanel), "=r"(pulse) : "0"(raisingPanel), "1"(pulse), "r"(left));
            top = 122 - pulse;
            right = 220;
            bottom = 124;
            raisingPanel->x0 = left;
            raisingPanel->y0 = top;
            raisingPanel->x1 = right;
            raisingPanel->y1 = top;
            raisingPanel->x2 = left;
            raisingPanel->y2 = bottom;
            ((RenderTexturedQuad *)raisingPanel)->x3 = right;
        }
        else {
            RenderTexturedQuad *base = D_800BE9F0;
            register u16 left asm("$3");
            register u16 right asm("$4");
            raisingPanel = &base[D_8009CDDC];
            asm volatile("" : "=r"(raisingPanel) : "0"(raisingPanel));
            left = 100;
            right = 220;
            raisingPanel->x0 = left;
            raisingPanel->y0 = left;
            raisingPanel->y1 = left;
            raisingPanel->x2 = left;
            bottom = 124;
            raisingPanel->x1 = right;
            raisingPanel->y2 = bottom;
            ((RenderTexturedQuad *)raisingPanel)->x3 = right;
        }
        raisingPanel->y3 = bottom;
        countdown = D_8009CE70;
        if (countdown == 0) {
            Anim_SetInterpRate(&D_8009D254->renderObject, 0x3C);
            D_8009D254->renderObject.flags_9C |= 2;
            Anim_SetInterpRate(&D_800B0CEC, 0x3C);
            {
                u8 state = D_8009CE74;
                D_8009CE70 = 0x3C;
                state++;
                D_800B0D88 |= 2;
                D_8009CE74 = state;
            }
        }
        else {
            countdown--;
            goto storeCountdown;
        }
        break;
    }
    case 4: {
        register RenderTexturedQuad *base asm("$7");
        register u16 right asm("$6");
        if ((D_8009D254->renderObject.variant_visible == 0) && (D_800B0D8A == 0)) {
            nextStage = D_8009CE74 + 1;
            storeNextStage:
            D_8009CE74 = nextStage;
            break;
        }
        base = D_800BE9F0;
        right = 220;
        fadeBrightness = D_8009CE70 * 2;
        fadingPanel = &base[D_8009CDDC];
        fadingPanel->color.bytes.r = fadeBrightness;
        fadingPanel->x0 = 0x64;
        fadingPanel->y0 = 0x64;
        fadingPanel->y1 = 0x64;
        fadingPanel->x2 = 0x64;
        fadingPanel->x1 = right;
        fadingPanel->y2 = 0x7C;
        fadingPanel->x3 = right;
        fadingPanel->y3 = 0x7C;
        base[D_8009CDDC].color.bytes.g = fadeBrightness;
        base[D_8009CDDC].color.bytes.b = fadeBrightness;
        countdown = D_8009CE70 - 1;
        storeCountdown:
        D_8009CE70 = countdown;
        break;
    }
    case 5:
        Battle_SetupPlayerPalette();
        D_8009D28C = -1;
        Sys_Shutdown();
        break;
    }
    AddPrim((u32 *)(D_800B0E38.ordering[D_8009CDDC] + 0x10),
    (u32 *)&D_800BE9F0[D_8009CDDC]);
}
