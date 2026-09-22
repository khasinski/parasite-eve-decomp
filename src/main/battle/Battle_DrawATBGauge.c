#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"

void Battle_DrawATBGauge(void)
{
    short ammo;
    register signed char i asm("$17");
    short y;
    RenderSpritePacket *sprite;
    BattleGaugePrim *packet;
    RenderSpritePacket *base;
    unsigned category;
    /* Keep the queue index initialized across the ammo-base callback. */
    i = 0;
    y = D_8009E358[g_ActiveDrawSlot * 3].y0 + 22;
    category = (D_8009D278->action->attackWord >> 20) & 3;
    ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
    base = &D_8009E768[0].sprite;
    packet = (BattleGaugePrim *)((char *)base - PE1_OFFSETOF(BattleGaugePrim, sprite));

    packet += g_ActiveDrawSlot;
    sprite = (RenderSpritePacket *)((char *)base + sizeof(BattleGaugePrim) * g_ActiveDrawSlot);
    ammo += *(unsigned short *)&D_8009D278->action->attackWord & 0x3ff;
    sprite->x = D_8009E358[g_ActiveDrawSlot * 3].x0 + 8;
    sprite->y = y;
    AddPrim((unsigned *)D_800B0E38.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    while (i < (signed char)Pad_GetMenuPressedBitOrDisabled()) {
        int kind = D_800BE830[i].field04;
        if (kind == 1) {
            --ammo;
        } else if (kind == 2) {
            unsigned mode = D_8009D278->action->turnWord & 0xc0;
            if (mode == 0xc0) {
                ammo -= *(unsigned char *)&D_8009D278->action->turnWord & 15;
            } else if (mode == 0x40) {
                --ammo;
            }
        } else if (kind == 0x189) {
            --ammo;
        }
        ++i;
    }
    if (ammo < 0) {
        ammo = 0;
    }
    Battle_DrawDecimalNumber(D_8009E7A0[g_ActiveDrawSlot], D_8009E358[g_ActiveDrawSlot * 3].x0 + 64, y - 1, ammo, 0);
}
