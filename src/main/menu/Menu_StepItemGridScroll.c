#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* The retail display clamps only the upper bound; negative totals survive. */
static inline int clampedStatTotal(ItemDataRecord *item, int index)
{
    int value = item->baseStats[index] + item->bonusStats[index];
    if (value > 999)
        value = 999;
    return value;
}

void Menu_StepItemGridScroll(ItemDataRecord *item, ItemDataRecord *other)
{
    int sprite;
    if (!other)
        return;
    if (item) {
        if (D_8009CF1C) {
            Draw_OffsetCursor(40, -50);
            Draw_AllocSprite(0x88);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-38, 12);
        }
        Draw_OffsetCursor(38, -12);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        if (D_8009CF1C) {
            Draw_OffsetCursor(12, -10);
            Draw_AllocSprite(0x88);
            Draw_SetStatCompareColor(item->bonusStats[0], other->bonusStats[0]);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(other->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[1], other->bonusStats[1]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[2], other->bonusStats[2]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[2]);
        } else {
            Draw_SetStatCompareColor(clampedStatTotal(item, 0), clampedStatTotal(other, 0));
            Draw_OffsetCursor(12, -2);
            Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 1), clampedStatTotal(other, 1));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 2), clampedStatTotal(other, 2));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
        }
    } else {
        sprite = D_8009CF18 ? 0x7C : 0x7F;
        Draw_OffsetCursor(4, 23);
        Draw_AllocSprite(sprite);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 1);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 2);
        Draw_SetColor(0x8080);
        Draw_OffsetCursor(40, -28);
        Draw_SetStatCompareColor(0, other->baseStats[0] + other->bonusStats[0]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[1] + other->bonusStats[1]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[2] + other->bonusStats[2]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
    }
    Draw_SetColor(0x808080);
}
