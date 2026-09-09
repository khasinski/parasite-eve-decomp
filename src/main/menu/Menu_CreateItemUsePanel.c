#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_OffsetPosition();
M2C_UNK MenuWidget_SetCurrentNode();
void *Inv_LookupData();
M2C_UNK Queue_Init();
void *MenuWidget_CreateSimpleNode();
void *MenuWidget_CreateNode();
M2C_UNK MenuWidget_ClearColumnLayout();
extern s32 g_InvItemUsableFlag;
extern void *g_MenuSelectedItemData;
void Menu_DrawEquipStatsPanel(void);
extern M2C_UNK Menu_HandleItemInput[];
#define Menu_HandleItemInput (Menu_HandleItemInput[0])
extern M2C_UNK Menu_DrawEquipItemName[];
#define Menu_DrawEquipItemName (Menu_DrawEquipItemName[0])
extern M2C_UNK Menu_DrawEquipItemIcon[];
#define Menu_DrawEquipItemIcon (Menu_DrawEquipItemIcon[0])
extern M2C_UNK Menu_DrawSoundEffectList[];
#define Menu_DrawSoundEffectList (Menu_DrawSoundEffectList[0])
extern M2C_UNK Menu_DrawEquipItemDetailPanel[];
#define Menu_DrawEquipItemDetailPanel (Menu_DrawEquipItemDetailPanel[0])

void Menu_CreateItemUsePanel(void) {
    s32 width;
    void *node;
    void *created;
    void *data;
    void *temp_v0;

    temp_v0 = Inv_LookupData();
    g_MenuSelectedItemData = temp_v0;
    if ((u8) M2C_FIELD(temp_v0, u8 *, 6) < 0xAU) {
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
        Draw_SetPrimCallback(created, M2C_FIELD(data, u8 *, 0x14));
        g_InvItemUsableFlag = M2C_FIELD(g_MenuSelectedItemData, u8 *, 6) != 9;
    } else {
        node = MenuWidget_CreateSimpleNode(0x37, 0, 0, 0);
        M2C_FIELD(node, M2C_UNK **, 0x30) = &Menu_DrawEquipItemIcon;
        M2C_FIELD(node, M2C_UNK **, 0x2C) = &Menu_HandleItemInput;
        MenuWidget_SetCurrentNode(node);
    }
    M2C_FIELD(MenuWidget_CreateSimpleNode(0x13, 0, 0, 0), M2C_UNK **, 0x30) = &Menu_DrawEquipItemName;
    Queue_Init();
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Menu_DrawEquipStatsDelta(void *);                      /* extern */
M2C_UNK Draw_OffsetCursor(M2C_UNK, M2C_UNK);            /* extern */
M2C_UNK Draw_AllocSprite(M2C_UNK);                     /* extern */
M2C_UNK Draw_PrintNumberWidth4Unk(u8);                          /* extern */
M2C_UNK Draw_PrintSignedNumberWidth4(s16);                         /* extern */
extern void *g_MenuSelectedItemData;

void Menu_DrawEquipStatsPanel(void) {
    void *temp_s0;

    Menu_DrawEquipStatsDelta(g_MenuSelectedItemData);
    temp_s0 = g_MenuSelectedItemData;
    if (temp_s0 != NULL) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 9));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0x12));
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 8));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0x10));
        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(M2C_FIELD(temp_s0, u8 *, 7));
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(M2C_FIELD(temp_s0, s16 *, 0xE));
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}
