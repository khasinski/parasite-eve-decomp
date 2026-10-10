#include "pe1/menu_item_record.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/menu_widget.h"
#include "pe1/inventory.h"
#include "../../../tools/m2c/m2c_macros.h"

/* Item use/discard panel: creation, its input handler and the item name
 * header. Defines the selected item record and the discard flag. */

#define NULL ((void *)0)
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Queue_Init(void);
extern s32 g_InvItemUsableFlag;
ItemDataRecord *g_MenuSelectedItemData;
void Menu_DrawEquipStatsPanel(void);
int Menu_HandleItemInput(int arg0, int arg1);
void Menu_DrawEquipItemName(void);
M2C_UNK Menu_DrawEquipItemIcon();
M2C_UNK Menu_DrawSoundEffectList();
M2C_UNK Menu_DrawEquipItemDetailPanel();
int g_MenuItemDiscardMode;
void Inv_SetActiveList(int mode, int *slot);
void Menu_PlayConfirmSound(void);
u8 *Str_LookupTableC(unsigned int index);
void Draw_PrintRawText(u8 *arg0);

void Menu_CreateItemUsePanel(int itemId) {
    s32 width;
    void *node;
    void *created;
    ItemDataRecord *data;
    ItemDataRecord *temp_v0;

    temp_v0 = Inv_LookupData(itemId);
    g_MenuSelectedItemData = temp_v0;
    if (temp_v0->kind < 0xAU) {
        node = MenuWidget_CreateSimpleNode(5, 0, 0, 0);
        created = MenuWidget_CreateNode(5, node, node);
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawEquipStatsPanel;
        M2C_FIELD(node, M2C_UNK **, 0x2C) = &Menu_HandleItemInput;
        width = 0x80;
        M2C_FIELD(node, s32 *, 0x34) = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        M2C_FIELD(created, M2C_UNK **, 0x30) = &Menu_DrawSoundEffectList;
        MenuWidget_ClearColumnLayout(created);
        M2C_FIELD(created, s32 *, 0x70) = -1;
        MenuWidget_SetCurrentNode(node);
        node = MenuWidget_CreateSimpleNode(6, 0, 0, 0);
        M2C_FIELD(node, s32 *, 0x34) = width;
        MenuWidget_OffsetPosition(node, 0x44, 0x14);
        created = MenuWidget_CreateNode(6, node, node);
        M2C_FIELD(created, M2C_UNK **, 0x30) = &Menu_DrawEquipItemDetailPanel;
        MenuWidget_ClearColumnLayout(created);
        data = g_MenuSelectedItemData;
        M2C_FIELD(created, s32 *, 0x3C) = 0x3E;
        Draw_SetPrimCallback(created, data->tailCount);
        g_InvItemUsableFlag = g_MenuSelectedItemData->kind != 9;
    } else {
        node = MenuWidget_CreateSimpleNode(0x37, 0, 0, 0);
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawEquipItemIcon;
        M2C_FIELD(node, M2C_UNK **, 0x2C) = &Menu_HandleItemInput;
        MenuWidget_SetCurrentNode(node);
    }
    M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawEquipItemName;
    Queue_Init();
}

M2C_UNK Menu_DrawEquipStatsDelta(void *);                      /* extern */
M2C_UNK Draw_OffsetCursor(M2C_UNK, M2C_UNK);            /* extern */
M2C_UNK Draw_AllocSprite(M2C_UNK);                     /* extern */
M2C_UNK Draw_PrintNumberWidth4Unk(u8);                          /* extern */
M2C_UNK Draw_PrintSignedNumberWidth4(s16);                         /* extern */

void Menu_DrawEquipStatsPanel(void) {
    ItemDataRecord *temp_s0;

    Menu_DrawEquipStatsDelta(g_MenuSelectedItemData);
    temp_s0 = g_MenuSelectedItemData;
    if (temp_s0 != NULL) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[2]);
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[1]);
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(temp_s0->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(temp_s0->bonusStats[0]);
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}

int Menu_HandleItemInput(int arg0, int arg1) {
    int value;
    ItemDataRecord *ptr;

    if ((arg1 & 0x10040) != 0) {
        if (g_MenuItemDiscardMode != 0) {
            ptr = g_MenuSelectedItemData;
            g_MenuItemDiscardMode = 0;
            value = ptr->itemId;
            Inv_SetActiveList(0, &value);
        } else {
            Inv_SetActiveList(9, 0);
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_DrawEquipItemName(void) {
    Draw_OffsetCursor(4, 4);
    Draw_PrintRawText(Str_LookupTableC(g_MenuSelectedItemData->itemId - 1));
}
