#include "common.h"
#include "pe1/inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_AllocSprite(int glyph);
void Draw_OffsetCursor(int x, int y);
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

extern unsigned char g_CursorRenderDataBlock[];
void *Str_LookupTable8(unsigned int arg0);
void Sfx_CursorRenderData(ItemDataRecord *record) {
    u8 *cursor;

    if (record->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) {
        cursor = g_CursorRenderDataBlock;
        if (record->kind == ITEM_KIND_ARMOR) {
            cursor = g_CursorRenderDataBlock + 0x10;
        }
    } else {
        cursor = Str_LookupTable8(record->itemId - 1);
    }

    Sfx_DrawSlotRow(record, cursor);
}

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void *Str_LookupTable8(u32);
extern s32 g_InvCategoryBaseItemId;
extern s16 *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern u8 g_KeyItemDataTable[] __asm__("g_KeyItemDataTable");
extern u8 g_EquipItemDataTable[] __asm__("g_EquipItemDataTable");

void Sfx_DrawActiveListSlot(s32 arg0) {
    s16 temp_v1;
    s32 temp_v1_3;
    register s32 temp_a1 asm("$5");
    u32 temp_a0;
    register u32 var_a0 asm("$4");
    void *temp_v1_2;
    void *var_a1;
    void *var_v0;
    void *saved_a1;

    temp_v1 = g_InvItemPtr[arg0];
    var_a1 = NULL;
    if ((u32) (temp_v1 - 0x100) < 0x80U) {
        temp_v1_2 = (temp_v1 << 5) + g_EquipItemDataTable;
        if (M2C_FIELD(temp_v1_2, u8 *, 5) & 0x10) {
            var_a1 = g_EquipItemDataTable + 0x31F8;
            if (M2C_FIELD(temp_v1_2, u8 *, 6) == 9) {
                var_a1 = g_EquipItemDataTable + 0x3208;
            }
        } else {
            var_a0 = M2C_FIELD(temp_v1_2, u8 *, 4) - 1;
            goto block_8;
        }
    } else {
        var_a0 = temp_v1 - 1;
        if (var_a0 >= 0xFFU) {
            if ((u32) (temp_v1 - 0x200) < 9U) {
                var_a0 = (g_InvCategoryBaseItemId + temp_v1) - 0x201;
                goto block_8;
            }
        } else {
block_8:
            var_a1 = Str_LookupTable8(var_a0);
        }
    }
    saved_a1 = var_a1;
    if ((arg0 >= 0) && (arg0 < g_InvSlotLimit)) {
        temp_v1_3 = g_InvItemPtr[arg0];
        temp_a1 = temp_v1_3;
        if ((u32) (temp_v1_3 - 0x100) < 0x80U) {
            var_v0 = (temp_v1_3 << 5) + g_EquipItemDataTable;
        } else {
            temp_a0 = temp_v1_3 - 1;
            if (temp_a0 < 0xFFU) {
                var_v0 = Item_LookupBaseData(temp_a0);
            } else if ((u32) (temp_a1 - 0x200) < 9U) {
                temp_v1_3 = temp_a1 << 5;
                var_v0 = temp_v1_3 + g_KeyItemDataTable;
            } else {
                goto block_17;
            }
        }
    } else {
block_17:
        var_v0 = NULL;
    }
    if (var_v0 != NULL) {
        Sfx_DrawSlotRow((ItemDataRecord *)var_v0, saved_a1);
    }
}
