#include "pe1/battle.h"

extern BattleEntity *g_PlayerEntity;
extern short g_AyaHpCurrent;

void BattleCmd_SetCurrentHP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity->core;
        if (current != 0) {
            current->curHP = arg0;
        }
    }

    g_AyaHpCurrent = arg0;
}
