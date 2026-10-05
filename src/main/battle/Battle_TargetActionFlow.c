/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
#include "pe1/battle_runtime.h"
#include "pe1/akao.h"
#include "pe1/render_object.h"
#include "pe1/psyq_nop.h"
#include "pe1/battle_cmd.h"
#include "pe1/menu_inventory.h"
#include "pe1/scene_transition.h"


s8 g_BattleTargetIndex;

static inline void PlayTargetSound(void) {
    void * volatile *slot = &g_AkaoBgmHandle;
    if (*slot) Akao_SendTableCommand(*slot, 0x44E, 0, 0x80, 0x7F);
}
void Battle_CycleTarget(s8 mode) {
    if (D_8009D2B0 >= 2) {
        if (mode != 1 && mode != 2) {
            if (D_8009D1F4 & 0x10) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + 1) % D_8009D2B0;
                Battle_InitFadeVars();
                PlayTargetSound();
            }
            if (D_8009D1F4 & 0x40) {
                Battle_MarkActiveEntities(g_BattleTargetList, g_BattleTargetIndex);
                g_BattleTargetIndex = (g_BattleTargetIndex + D_8009D2B0 - 1) % D_8009D2B0;
                Battle_InitFadeVars();
                PlayTargetSound();
            }
        }
    } else if (!D_8009D2B0) mode = 8;
    switch (mode) {
    case 0: case 1: case 2: case 3: {
        s8 outOfRange;
        BattleEntity *actor;
        Render_AnimationFrame();
        if ((D_8009D278->stateFlags & 0x30) == 0x10)
            outOfRange = g_BattleTargetList[g_BattleTargetIndex].dist > 200;
        else
            outOfRange = D_8009D278->action->range < g_BattleTargetList[g_BattleTargetIndex].dist;
        actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, outOfRange,
            ((EnemyCombatant *)actor->core)->field06.bytes.entityId);
        break;
    }
    case 4: case 5: case 6: case 7: {
        BattleEntity *actor = g_BattleTargetList[g_BattleTargetIndex].actor;
        Battle_DrawStatusOverlay(&actor->renderObject, mode, 0,
            ((EnemyCombatant *)actor->core)->field06.bytes.entityId);
        break;
    }
    case 8: Battle_DrawStatusOverlay(0, 8, 0, 0); break;
    }
    Battle_StepPlayerTurn(g_BattleTargetList, g_BattleTargetIndex, mode);
}

u8 D_8009D294;
u8 D_8009D1DC;
u8 D_8009CE38[4];
u8 D_8009D2D8;
u8 D_8009CE3C;

void Battle_SetupEntityTarget(BattleEntity *actor) {
    BattleEntity *target;
    D_8009D294 = 0;
    D_8009D1DC = D_8009D278->action->turnWord & 15;
    D_8009CE38[0] = D_8009D278->action->animMode[0];
    /* Preserve retail load delays for byte stores at nonzero GP offsets. */
    {
        u8 value = D_8009D278->action->animMode[1];
        PE1_NOP_DEP("r", value);
        D_8009CE38[1] = value;
    }
    {
        u8 value = D_8009D278->action->animMode[2];
        PE1_NOP_DEP("r", value);
        D_8009CE38[2] = value;
    }
    {
        u8 value = D_8009D278->action->animMode[3];
        PE1_NOP_DEP("r", value);
        D_8009CE38[3] = value;
    }
    target = 0;
    if ((actor->entityFlags & 0x40000000) && !((EnemyCombatant *)actor->core)->deathPersist) {
        for (target = D_8009D20C; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009D254 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                /* Retail tests actor here, not the candidate target. */
                target->teamId == actor->teamId && !(actor->entityFlags & 0x40000000)) break;
        }
    } else if ((actor->entityFlags & 0x6000) && (s8)((EnemyCombatant *)actor->core)->field04.bytes.field05 == 3) {
        for (target = D_8009D20C; target; target = target->next) {
            EnemyCombatant *enemy;
            if (target == D_8009D254 || target == actor) continue;
            enemy = target->core;
            if (enemy && enemy->hpAlive > 0 &&
                target->parent == actor && (s8)enemy->field04.bytes.field05 == 1) break;
        }
    } else if (!(actor->entityFlags & 0x4000)) target = actor;
    if (target) {
        unsigned int mode = D_8009D278->action->turnWord & 0xC0;
        if (mode == 0xC0 || mode == 0x40) {
            BattleInitSlot *slot;
            D_8009D1DC = 0;
            slot = &D_800BE830[D_8009CE3C];
            slot->actor = target;
            slot->field04 = 2;
            slot->field06 = (s8)D_8009D2D8;
            D_8009CE3C++;
        } else {
            while (D_8009D1DC) {
                BattleInitSlot *slot;
                s16 actionIndex = (s8)D_8009D2D8;
                D_8009D1DC--;
                slot = &D_800BE830[D_8009CE3C];
                slot->actor = target;
                slot->field04 = 1;
                slot->field06 = actionIndex;
                D_8009CE3C++;
            }
        }
        Battle_DispatchEntityEffect();
        D_8009D278->stateFlags |= 0x200000;
    }
}


s8 g_BattleActiveTurnSlot, g_BattleItemMenuState, g_BattleTargetIndex;
u8 D_8009D1DC, D_8009CE60, D_8009CE3C;

static inline void PlaySound(int command) {
    void * volatile *slot = &g_AkaoBgmHandle;
    if (*slot) Akao_SendTableCommand(*slot, command, 0, 0x80, 0x7F);
}
static inline void FinishAction(void) {
    u8 i;
    if ((unsigned int)((u16)D_800BE830[0].field04 - 3) < 404 && D_8009D254->actionMode >= 4)
        g_GameStateFlags |= 0x100;
    if ((unsigned int)((u16)D_800BE830[0].field04 - 387) >= 21) {
        for (i=0; i<D_8009CE3C; i++) {
            if ((unsigned int)((u16)D_800BE830[i].field04 - 1) < 2) {
                Battle_DispatchEntityEffect();
                break;
            }
        }
    }
    D_8009D288 = 0;
    D_8009D278->hpAlive = 0;
    D_8009D278->stateFlags |= 0x200000;
    BattleCmd_ResetTableCursor();
}
int Battle_HandleItemMenu(void) {
    u8 active=1;
    u8 opened=0, cancelled=0;
    if (!g_BattleActiveTurnSlot) {
        D_8009D1DC=0;
        active=0;
        FinishAction();
        Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
        PlaySound(0x44C);
    } else {
        if ((D_8009D1F4&0x2000) && !MenuWidget_FindByModeAndSelectedBase(1,0) && g_BattleItemMenuState!=2) {
            cancelled=1;
            if (D_8009D1DC==(D_8009D278->action->turnWord&15))D_8009CE60=1;
            else D_8009CE60=0;
            if (g_BattleItemMenuState==1) MenuWidget_InitPool();
            else if (g_BattleItemMenuState==2) BattleCmd_UndoPending();
            if (--g_BattleActiveTurnSlot) {
                D_8009D1DC=D_8009D278->action->turnWord&15;
            } else {
                active=0;
                FinishAction();
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PlaySound(0x44C);
                return active;
            }
            PlaySound(0x44C);
        }
        if (!g_BattleItemMenuState) {
            if (g_BattleActiveTurnSlot) {
                int mode;
                if (D_8009D278->action->actionCode.actionId==8)mode=3;
                else {
                    unsigned int actionMode=(D_8009D278->action->turnWord>>6)&3;
                    switch(actionMode) {
                    case 1: mode=1; break;
                    case 3: mode=2; break;
                    default: mode=0; break;
                    }
                }
                Battle_CycleTarget(mode);
                if ((D_8009D1F4&0x200)&&!cancelled) {
                    D_8009CE60=0;
                    if(D_8009D2B0) {
                        Battle_FillActionQueue(&g_BattleTargetList[g_BattleTargetIndex]);
                        if(!D_8009D1DC) {
                            g_BattleActiveTurnSlot--;
                            D_8009D1DC=D_8009D278->action->turnWord&15;
                        }
                    }
                    PlaySound(0x44C);
                }
            }
            if(D_8009D1DC==(D_8009D278->action->turnWord&15) && (D_8009D1F4&0x80) && g_BattleActiveTurnSlot && !cancelled) {
                g_BattleItemMenuState=1;
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                PlaySound(0x44C);
                Inventory_OpenAyaItemList(1);
                opened=1;
                Render_BeginSceneLoad();
            }
            if ((D_8009D1F4&0x400)&&!opened&&!cancelled) {
                Battle_MarkActiveEntities(g_BattleTargetList,g_BattleTargetIndex);
                if(D_8009CE3C) {Battle_AdvanceTurnSlot();Battle_InitFadeVars();}
                else active=0;
                PlaySound(0x44D);
            }
        } else {
            g_BattleItemMenuState=Battle_StepEnemyTurn(g_BattleItemMenuState);
            if(g_BattleItemMenuState==-1) {
                g_BattleItemMenuState=0;
                active=0;
                FinishAction();
            } else if(!g_BattleItemMenuState) Battle_InitFadeVars();
        }
    }
    return active;
}
