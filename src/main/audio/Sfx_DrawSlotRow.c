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
