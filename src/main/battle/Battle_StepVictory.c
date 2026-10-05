/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
/* COMMON metadata retains retail GP-relative addressing; symbols provide storage. */
u8 D_8009CE70, D_8009CE74;
Combatant *D_8009D278;
int D_8009D28C;
u8 D_8009D2A0;
void Aya_DeriveStats(int *maximum, int *current);
void Battle_StartDeathAnim(void);
/* Eight-stage transition: model fades, an effect spawn, stat recovery and
 * HUD reset. Matching debt: nine pins, one empty pointer barrier, shared goto
 * tails, interior HUD symbols, and a signed byte view of D_8009D2A0.
 * The object pointer is recovered from its known variant_visible field. */
int Battle_StepVictory(void)
{
    register int result asm("$18") = 0;
    register BattleEntity *player;
    BattleEntity *spawnPlayer;
    Combatant *core;
    register RenderObjectEntity *object asm("$16");
    register u8 brightness asm("$16");
    u8 countdown;
    u8 nextStage;
    register u8 remaining;
    register Combatant *restoredCore;
    switch (D_8009CE74) {
    case 0:
        player = D_8009D254;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009D254;
        }
        if (player->animLastFrame != (u16)(player->animFrame >> 16))
            goto clearFlag;
        D_8009CE70 = 16;
        player->entityFlags |= 0x100;
        D_8009CE74++;
        break;
    case 1:
        brightness = D_8009CE70;
        if (brightness != 0) {
            brightness = ~(brightness << 3);
            goto fade;
        }
        Render_FadeEntityColor(&D_8009D254->renderObject, 255, 255, 255);
        object = &D_800B0CEC;
        Render_FadeEntityColor(object, 255, 255, 255);
        Anim_SetInterpRate(&D_8009D254->renderObject, 30);
        D_8009D254->renderObject.flags_9C |= 2;
        Anim_SetInterpRate(object, 30);
        D_800B0D88 |= 2;
        D_8009CE74++;
        break;
    case 2:
        if (D_8009D254->renderObject.variant_visible != 0 || D_800B0D8A != 0)
            break;
        Scene_LoadRoomAssets(0x69, D_8009D254);
        spawnPlayer = D_8009D254;
        Asset_Find08Alt(0x4AF, 0, spawnPlayer->posX.parts.integer, spawnPlayer->posY.parts.integer, spawnPlayer->posZ.parts.integer);
        {
            register u8 delay asm("$3") = 0x34;
            nextStage = D_8009CE74 + 1;
            D_8009CE70 = delay;
        }
        goto advance;
    case 3:
        if (D_8009CE70 != 0) {
            remaining = D_8009CE70 - 1;
            goto decrement;
        }
        D_8009D254->renderObject.variant_visible = 1;
        Anim_SetInterpRate(&D_8009D254->renderObject, 30);
        D_8009D254->renderObject.flags_9C |= 4;
        Render_FadeEntityColor(&D_8009D254->renderObject, 255, 255, 255);
        {
            register u8 *visible = &D_800B0D8A;
            asm volatile("" : "=r"(visible) : "0"(visible));
            *visible = 1;
            object = (RenderObjectEntity *)(visible - PE1_OFFSETOF(RenderObjectEntity, variant_visible));
        }
        Anim_SetInterpRate(object, 30);
        D_800B0D88 |= 4;
        Render_FadeEntityColor(object, 255, 255, 255);
        Entity_SetActionMode(D_8009D254, 0xF);
        D_8009CE70 = 30;
        D_8009D254->animFrame = 0x230000;
        D_8009D254->animPrev.fixed = 0x220000;
        D_8009CE74++;
        break;
    case 4:
        countdown = D_8009CE70;
        if (countdown == 16) {
            D_8009CE74++;
            player = D_8009D254;
clearFlag:
            player->entityFlags &= ~0x100;
        } else {
            D_8009CE70 = countdown - 1;
        }
        break;
    case 5:
        brightness = D_8009CE70;
        if (brightness != 0) {
            brightness = (brightness << 3) + 127;
fade:
            Render_FadeEntityColor(&D_8009D254->renderObject, brightness, brightness, brightness);
            Render_FadeEntityColor(&D_800B0CEC, brightness, brightness, brightness);
            remaining = D_8009CE70 - 1;
decrement:
            D_8009CE70 = remaining;
        } else {
            D_8009CE74++;
            D_8009D254->renderObject.flags_9C |= 0x20;
        }
        break;
    case 6:
        player = D_8009D254;
        if (player->animLastFrame != player->animPrev.parts.integer)
            break;
        Entity_SetActionMode(player, D_8009D278->actionMode12);
        nextStage = D_8009CE74 + 1;
advance:
        D_8009CE74 = nextStage;
        break;
    case 7:
        core = D_8009D278;
        core->curHP = (s16)core->maxHP >> 1;
        core->exp_or_acc >>= 2;
        Battle_ResetEnemyStats(1);
        restoredCore = D_8009D278;
        player = D_8009D254;
        restoredCore->panelB_val = restoredCore->curHP;
        restoredCore->panelB_x = player->renderObject.projected_x;
        restoredCore->panelB_y = player->renderObject.projected_y;
        restoredCore->panelB_timer = 30;
        D_8009D278->panelB_flag = 1;
        Aya_DeriveStats(&D_8009D278->atbStep, &D_8009D278->atbRate);
        {
            register u8 initialGreen asm("$6") = 70;
            register u8 blue asm("$5") = 130;
            register u8 u asm("$4") = 159;
            register u8 v asm("$2") = 255;
            register u8 clutLow asm("$3") = 249;
            D_800B00ED[0] = initialGreen;
            D_800B00FD[0] = initialGreen;
            D_800B0111[0] = initialGreen;
            D_800B0121[0] = initialGreen;
            initialGreen = 54;
            D_800B00F4[0] = u;
            D_800B0104[0] = u;
            D_800B692C[0] = u;
            D_800B0118[0] = u;
            D_800B0128[0] = u;
            D_800B6948[0] = u;
            D_800B00EC[0] = 0;
            D_800B00EE[0] = blue;
            D_800B00F5[0] = v;
            D_800B00F6[0] = clutLow;
            D_800B00FC[0] = 0;
            D_800B00FE[0] = blue;
            D_800B0105[0] = v;
            D_800B0106[0] = clutLow;
            D_800B692D[0] = v;
            D_800B692E[0] = clutLow;
            D_800B0110[0] = 0;
            D_800B0112[0] = blue;
            D_800B0119[0] = v;
            D_800B011A[0] = clutLow;
            D_800B0120[0] = 0;
            D_800B0122[0] = blue;
            D_800B0129[0] = v;
            D_800B012A[0] = clutLow;
            D_800B6949[0] = v;
            D_800B694A[0] = clutLow;
            D_800B0134[0] = 0;
            D_800B0135[0] = blue;
            D_800B0136[0] = initialGreen;
            D_800B013C[0] = 74;
            D_800B013D[0] = v;
            D_800B013E[0] = 59;
            D_800B0144[0] = 0;
            D_800B0145[0] = blue;
            D_800B0146[0] = initialGreen;
            D_800B014C[0] = 74;
            D_800B014D[0] = v;
            D_800B014E[0] = 59;
            D_800B017C[0] = 0;
            D_800B017D[0] = blue;
            D_800B017E[0] = initialGreen;
            D_800B0184[0] = 74;
            D_800B0185[0] = v;
            D_800B0186[0] = 59;
            D_800B018C[0] = 0;
            D_800B018D[0] = blue;
            D_800B018E[0] = initialGreen;
            D_800B0194[0] = 74;
            D_800B0195[0] = v;
            D_800B0196[0] = 59;
        }
        D_8009CE74 = 0;
        D_8009D28C = 0;
        result = 1;
        if (*(s8 *)&D_8009D2A0 == 0)
            Battle_StartDeathAnim();
        break;
    }
    return result;
}
