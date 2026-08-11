#include "pe1/battle_runtime.h"

typedef struct EnemyAnimTable {
    u8 rows[9][5];
} EnemyAnimTable;

typedef struct EnemyOffenseScaleTable {
    u8 values[9];
} EnemyOffenseScaleTable;

extern const EnemyAnimTable D_800106A4;
extern const EnemyOffenseScaleTable D_800106D4;

void Battle_SetupEnemyAnims(void) {
    EnemyAnimTable animTable;
    EnemyOffenseScaleTable scaleTable;
    BattleAction *action;
    Combatant *combatant;
    u8 *animRow;
    unsigned int actionId;

    animTable = D_800106A4;
    scaleTable = D_800106D4;

    combatant = D_8009D278;
    action = combatant->action;
    actionId = action->actionCode.bytes.actionIdByte;
    combatant->scaledOffense =
        (combatant->stat22 * scaleTable.values[actionId]) / 10;
    combatant->actionMode12 = 4;
    D_8009D278->actionMode13 = 5;
    D_8009D278->actionMode14 = 6;
    D_8009D278->actionMode17 = 7;
    D_8009D278->actionMode15 = 8;
    D_8009D278->actionMode18 = 9;
    D_8009D278->actionMode16 = 10;
    D_8009D278->actionMode19 = 11;

    animRow = animTable.rows[actionId];
    D_8009D278->action->animMode[0] = animRow[0];
    D_8009D278->action->animMode[1] = animRow[1];
    D_8009D278->action->animMode[2] = animRow[2];
    D_8009D278->action->animMode[3] = animRow[3];
    D_8009D278->action->field08 = animRow[4];
    Scene_SetStoryDay(D_8009D278->action->actionCode.actionId);
}
