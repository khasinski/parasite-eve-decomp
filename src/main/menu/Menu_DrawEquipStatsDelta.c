/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct EquipStatPreview {
    u8 pad0[7];
    u8 attack;
    u8 range;
    u8 bullets;
    u8 padA[4];
    s16 attack_delta;
    s16 range_delta;
    s16 bullets_delta;
} EquipStatPreview;

typedef struct MenuWidgetNode {
    u8 pad0[0x24];
    s32 kind;
} MenuWidgetNode;

extern s32 D_8009CF18;
extern s32 D_8009CF1C;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int sprite);
void Draw_PrintNumberWidth4(int value);
MenuWidgetNode *MenuWidget_GetCurrentNode(void);

void Menu_DrawEquipStatsDelta(EquipStatPreview *preview) {
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
        if (node->kind == 7) {
            Draw_OffsetCursor(0x18, 0xE);
            return;
        }
    }

    Draw_OffsetCursor(0x1E, -0x1C);
    value = preview->attack + preview->attack_delta;
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->range + preview->range_delta;
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->bullets + preview->bullets_delta;
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
}
