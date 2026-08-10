#include "common.h"
#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern s32 D_8009CF18;
extern s32 D_8009CF1C;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int sprite);
void Draw_PrintNumberWidth4(int value);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);

void Menu_DrawEquipStatsDelta(ItemDataRecord *preview) {
    int sprite_base;
    int value;
    MenuWidgetNode *node;

    if (preview == 0) {
        return;
    }

    sprite_base = 0x7F;
    if (D_8009CF18 != 0) {
        sprite_base = 0x7C;
    }

    Draw_OffsetCursor(4, 0x1C);
    Draw_AllocSprite(sprite_base);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 1);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 2);

    if (D_8009CF1C != 0) {
        node = MenuWidget_GetCurrentNode();
        if (node->selected_base == 7) {
            Draw_OffsetCursor(0x18, 0xE);
            return;
        }
    }

    Draw_OffsetCursor(0x1E, -0x1C);
    value = preview->baseStats[0] + preview->bonusStats[0];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[1] + preview->bonusStats[1];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[2] + preview->bonusStats[2];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
}
