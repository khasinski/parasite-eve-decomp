#include "common.h"
#include "pe1/inventory.h"
#include "pe1/text.h"
#include "pe1/save.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_AllocSprite(int glyph);
void Draw_PrintRawText(u8 *text);
void Draw_PrintNumberWidth3Unk(int value);

void Sfx_DrawSlotRow(ItemDataRecord *entry, u8 *text) {
    int type;
    int sprite;
    int ammo;

    Draw_StatePush();

    if (entry->kind < 8) {
        sprite = entry->baseStats[2] + entry->bonusStats[2];
        ammo = entry->ammo;
        if (sprite < 1000) {
            if (ammo == sprite) {
                Draw_AllocSprite(0x69);
            }
        } else if (ammo == 999) {
            Draw_AllocSprite(0x69);
        }
    }

    Draw_AllocSprite(((u8 *)entry)[0] - 1);
    Draw_OffsetCursor(0x12, 0);

    if (text != 0) {
        Draw_PrintRawText(text);
    }

    type = entry->kind;
    if (type != 0 && (unsigned int)type < 8) {
        if (type - 4 <= 0) {
            goto draw_ammo;
        }
        if (type != 4) {
            goto draw_ammo;
        }
        goto done;
    } else {
        int cmp;
        if ((unsigned int)type < 0x13 || (cmp = type - 0x12) == 0) {
            goto done;
        }
    }

draw_ammo:
    Draw_OffsetCursor(0x56, 0);

    type = entry->kind;
    if (type != 0 && (unsigned int)type < 8) {
        if (type - 4 > 0) {
            sprite = type + 0x1A;
        } else {
            sprite = 0x1F;
        }
    } else if ((unsigned int)type < 0x13) {
        sprite = 0x1E;
    } else {
        sprite = type + 0xC;
    }

    Draw_AllocSprite(sprite);
    Draw_OffsetCursor(0, 5);
    Draw_PrintNumberWidth3Unk(entry->ammo);

done:
    Draw_StatePop();
}

void Sfx_CursorRenderData(ItemDataRecord *record) {
    u8 *cursor;

    if (record->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) {
        cursor = g_CursorRenderMetadataWindows[0].text;
        if (record->kind == ITEM_KIND_ARMOR) {
            cursor = g_CursorRenderMetadataWindows[1].text;
        }
    } else {
        cursor = Str_LookupTable8(record->itemId - 1);
    }

    Sfx_DrawSlotRow(record, cursor);
}

#define NULL ((void *)0)

extern s32 g_InvCategoryBaseItemId;
extern s16 *g_InvItemPtr;
extern s32 g_InvSlotLimit;

void Sfx_DrawActiveListSlot(s32 arg0) {
    s16 temp_v1;
    s32 temp_v1_3;
    register s32 temp_a1 asm("$5");
    u32 temp_a0;
    u32 var_a0;
    ItemDataRecord *temp_v1_2;
    u8 *var_a1;
    ItemDataRecord *var_v0;
    u8 *saved_a1;

    temp_v1 = g_InvItemPtr[arg0];
    var_a1 = NULL;
    if ((temp_v1 - 0x100) < 0x80U) {
        temp_v1_2 = &((ItemDataRecord *)g_EquipItemDataTable)[temp_v1];
        if (temp_v1_2->flags & 0x10) {
            var_a1 = g_EquipItemDataTable + 0x31F8;
            if (temp_v1_2->kind == 9) {
                var_a1 = g_EquipItemDataTable + 0x3208;
            }
        } else {
            var_a0 = temp_v1_2->itemId - 1;
            var_a1 = Str_LookupTable8(var_a0);
        }
    } else {
        var_a0 = temp_v1 - 1;
        if (var_a0 >= 0xFFU) {
            if ((temp_v1 - 0x200) < 9U) {
                var_a0 = (g_InvCategoryBaseItemId + temp_v1) - 0x201;
                var_a1 = Str_LookupTable8(var_a0);
            }
        } else {
            var_a1 = Str_LookupTable8(var_a0);
        }
    }
    saved_a1 = var_a1;
    if ((arg0 >= 0) && (arg0 < g_InvSlotLimit)) {
        temp_v1_3 = g_InvItemPtr[arg0];
        temp_a1 = temp_v1_3;
        if ((temp_v1_3 - 0x100) < 0x80U) {
            var_v0 = &((ItemDataRecord *)g_EquipItemDataTable)[temp_v1_3];
        } else {
            temp_a0 = temp_v1_3 - 1;
            if (temp_a0 < 0xFFU) {
                var_v0 = Item_LookupBaseData(temp_a0);
            } else if ((temp_a1 - 0x200) < 9U) {
                temp_v1_3 = temp_a1 << 5;
                var_v0 = (ItemDataRecord *)(temp_v1_3 + g_KeyItemDataTable);
            } else {
                var_v0 = NULL;
            }
        }
    } else {
        var_v0 = NULL;
    }
    if (var_v0 != NULL) {
        Sfx_DrawSlotRow(var_v0, saved_a1);
    }
}
