
#include "pe1/battle.h"

extern int g_GameStateFlags;
extern BattleEntity *g_PlayerEntity;
extern int g_ActiveActor;

void Asset_Find08Alt(int id, int arg1, int x, int y, int z);
void Battle_CopyPadStateToRecord(void);
void Battle_SetupEnemyAnims(void);
void Battle_SyncEnemyAttributes(void);

void Battle_DispatchSpecialAction(int arg0) {
    BattleEntity *actor;

    if ((g_GameStateFlags & 2) == 0) {
        actor = g_PlayerEntity;
        g_ActiveActor = (int)actor->core;
        Asset_Find08Alt(0x453, 1, actor->posX.parts.integer,
                       actor->posY.parts.integer, actor->posZ.parts.integer);
    }

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        ((Combatant *)g_ActiveActor)->actionMode12 = 0xD;
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    }
}
