/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/battle_runtime.h"

extern volatile struct { Combatant *value; char pad[12]; } D_8009D278_absolute __asm__("D_8009D278");
#define ACTIVE (D_8009D278_absolute.value)

void Battle_StartEnemyAttackEffect(BattleEntity *entity)
{
    Combatant *core = (Combatant *)entity->core;
    u32 *status = (u32 *)&core->statusFlags2;
    int effect;

    if ((ACTIVE->action->turnWord & 0x400) && ((*status & 3) != 1)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 2, 0, 0);
        Asset_Find08Alt(0x484, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x300) == 0x300 &&
        ((*status & 0x3C000) != 0x14000)) {
        D_8009D208 = Scene_LoadRoomAssets(90, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x100) &&
        ((*status & 0xC000) != 0x4000)) {
        D_8009D208 = Scene_LoadRoomAssets(88, entity);
        Asset_Find08Alt(0x488, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x200) &&
        ((*status & 0x30000) != 0x10000)) {
        D_8009D208 = Scene_LoadRoomAssets(89, entity);
        Asset_Find08Alt(0x486, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x800) &&
        ((*status & 0xC) != 4)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 1, 0, 0);
        Asset_Find08Alt(0x482, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
        return;
    }
    if ((ACTIVE->action->turnWord & 0x1000) &&
        ((*status & 0x30) != 0x10)) {
        effect = Scene_LoadRoomAssets(7, entity);
        D_8009D208 = effect;
        Pm_SendCmd(effect, 0, 0, 0, 0, 0);
        Asset_Find08Alt(0x480, 0, entity->renderObject.target_x,
                        entity->renderObject.target_y, entity->renderObject.target_z);
    }
}
