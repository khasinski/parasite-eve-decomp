/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/battle_runtime.h"
#include "pe1/battle_cmd.h"
#include "pe1/akao.h"
#include "pe1/menu_inventory.h"
#include "pe1/scene_transition.h"

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
