#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

void Battle_DrawATBGauge(void) {
    BattleAction *action;
    BattleGaugePrim *gauge;
    BattleStatusLinePrim *line;
    int ammo;
    int entryType;
    int index;
    int turnMode;

    line = &D_8009E358[g_ActiveDrawSlot * 3];
    action = D_8009D278->action;
    gauge = &D_8009E768[g_ActiveDrawSlot];
    gauge->x = line->x0 + 8;
    gauge->y = line->y0 + 0x16;

    ammo = Inv_GetWeaponCategoryAmmoBase(((action->attackWord >> 20) & 3) - 1);
    ammo += action->attackWord & 0x3ff;
    AddPrim(D_800B0E38[g_ActiveDrawSlot] + 4, (unsigned int *)gauge);

    index = 0;
    while ((s8)index < (s8)Pad_GetMenuPressedBitOrDisabled()) {
        entryType = D_800BE830[(s8)index].field04;
        if (entryType == 1) {
            ammo--;
        } else if (entryType == 2) {
            action = D_8009D278->action;
            turnMode = action->turnWord & 0xc0;
            if (turnMode == 0xc0) {
                ammo -= action->turnWord & 0xf;
            } else if (turnMode == 0x40) {
                ammo--;
            }
        }
        index++;
    }

    if ((s16)ammo < 0) {
        ammo = 0;
    }
    Battle_DrawDecimalNumber(D_8009E7A0[g_ActiveDrawSlot],
                             line->x0 + 0x40, (s16)(line->y0 + 0x15),
                             (s16)ammo, 0);
}
