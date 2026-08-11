#include "pe1/battle_runtime.h"

void Battle_StartEnemyAttackEffect(BattleEntity *entity) {
    EnemyCombatant *enemy;
    int handle;
    int effectId;

    enemy = entity->core;
    if ((D_8009D278->action->turnWord & 0x400) &&
        (enemy->statusFlags2 & 3) != 1) {
        handle = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = handle;
        Pm_SendCmd(handle, 0, 0, 2, 0, 0);
        effectId = 0x484;
    } else if ((D_8009D278->action->turnWord & 0x300) == 0x300 &&
               (enemy->statusFlags2 & 0x3c000) != 0x14000) {
        D_8009D208 = Scene_LoadRoomAssets(0x5a, entity);
        effectId = 0x488;
    } else if ((D_8009D278->action->turnWord & 0x100) &&
               (enemy->statusFlags2 & 0xc000) != 0x4000) {
        D_8009D208 = Scene_LoadRoomAssets(0x58, entity);
        effectId = 0x488;
    } else if ((D_8009D278->action->turnWord & 0x200) &&
               (enemy->statusFlags2 & 0x30000) != 0x10000) {
        D_8009D208 = Scene_LoadRoomAssets(0x59, entity);
        effectId = 0x486;
    } else if ((D_8009D278->action->turnWord & 0x800) &&
               (enemy->statusFlags2 & 0xc) != 4) {
        handle = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = handle;
        Pm_SendCmd(handle, 0, 0, 1, 0, 0);
        effectId = 0x482;
    } else if ((D_8009D278->action->turnWord & 0x1000) &&
               (enemy->statusFlags2 & 0x30) != 0x10) {
        handle = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = handle;
        Pm_SendCmd(handle, 0, 0, 0, 0, 0);
        effectId = 0x480;
    } else {
        return;
    }

    Asset_Find08Alt(effectId, 0, entity->renderObject.target_x,
                    entity->renderObject.target_y,
                    entity->renderObject.target_z);
}
