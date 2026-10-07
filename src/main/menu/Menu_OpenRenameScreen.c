#include "common.h"
#include "pe1/save.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode();
extern M2C_UNK D_80092354[];
#define D_80092354 (D_80092354[0])
extern M2C_UNK D_80092380[];
#define D_80092380 (D_80092380[0])
extern M2C_UNK D_800923A0[];
#define D_800923A0 (D_800923A0[0])
extern s32 g_GameState[];
#define g_GameState (g_GameState[0])
void Menu_DrawSaveMetadataPreview(void);
int Menu_StepNameEntry(MenuWidgetNode *parent, unsigned int flags);
int Menu_StepNameEntryAlt(MenuWidgetNode *parent, unsigned int flags);
void Menu_DrawItemIconList(int arg0);
void Menu_DrawMemCardSlot1List(int arg0);
void Menu_DrawMemCardSlot2List(int arg0);
void Menu_DrawMemCardSlot3List(int arg0);

void Menu_OpenRenameScreen(s32 arg0) {
    M2C_UNK *var_v1;
    s32 temp_v0_5;
    ItemDataRecord *record;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_3;
    MenuWidgetNode *temp_v0_4;
    MenuWidgetNode *var_a3;

    temp_v0 = MenuWidget_CreateSimpleNode(0x17, 0, 0, 0);
    (temp_v0)->update = (void (*)())Menu_StepNameEntryAlt;
    temp_v0_5 = Save_GetMetadataWindowIndex();
    var_v1 = &D_80092354;
    if (temp_v0_5 != 0) {
        var_v1 = &D_80092380;
    }
    M2C_FIELD(temp_v0, M2C_UNK **, 0x4C) = var_v1;
    if (Save_GetMetadataWindowIndex() != 0) {
        var_a3 = MenuWidget_CreateNode(0x18, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot2List;
        (var_a3)->y = ((var_a3)->y - 0x34);
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0, var_a3);
        (var_a3)->draw = &Menu_DrawMemCardSlot3List;
        (var_a3)->y = ((var_a3)->y - 0x34);
    } else {
        (MenuWidget_CreateNode(0x17, temp_v0, temp_v0))->draw = &Menu_DrawMemCardSlot1List;
        (MenuWidget_CreateNode(0x18, temp_v0, temp_v0))->draw = &Menu_DrawMemCardSlot2List;
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot3List;
    }
    (var_a3)->cursor_x = -1;
    temp_v0_3 = MenuWidget_CreateSimpleNode(0x11, MenuWidget_FindByModeAndSelectedBase(2, 0x17), 0, 0);
    __asm__ volatile("");
    var_a3 = MenuWidget_CreateNode(0x11, temp_v0_3, temp_v0_3);
    (temp_v0_3)->update = (void (*)())Menu_StepNameEntry;
    (var_a3)->draw = &Menu_DrawItemIconList;
    (var_a3)->cursor_x = 0;
    (var_a3)->cursor_y = 0;
    MenuWidget_SetCurrentNode(var_a3);
    temp_v0_4 = MenuWidget_CreateSimpleNode(0x1A, 0, 0, 0);
    __asm__ volatile("");
    (temp_v0_4)->draw = &Menu_DrawSaveMetadataPreview;
    record = Inv_LookupActiveListData(arg0);
    g_MenuRenameTargetRecord = record;
    if (record == 0) {
        M2C_FIELD(temp_v0_4, M2C_UNK **, 0x4C) = &D_800923A0;
    }
    Save_SelectMetadataWindow(g_MenuRenameTargetRecord);
    Save_LoadMetadataWindowText();
    Inv_ClearEquipFlagForKind(g_MenuRenameTargetRecord);
    g_GameState = g_GameState | 0x8000;
}

#include "common.h"
#include "pe1/save.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
void Sfx_CursorRenderData(ItemDataRecord *record);
#include "pe1/text.h"
#include "pe1/draw_state.h"
void Draw_PrintRawText(u8 *text);
void Draw_EmitWipeBarRect(int width, int height, int mode);

void Menu_DrawSaveMetadataPreview(void);

void Menu_DrawSaveMetadataPreview(void) {
    u8 *var_a0;
    s32 temp_v0;
    register s32 var_a0_2 asm("$4");
    s32 var_s0;
    s32 frame_pad[2];

    (void) frame_pad;
    if (g_MenuRenameTargetRecord != NULL) {
        Draw_OffsetCursor(0x1C, 0xC);
        Sfx_CursorRenderData(g_MenuRenameTargetRecord);
        var_a0 = (u8 *)g_CursorRenderMetadataWindows;
        if (g_MenuRenameTargetRecord->kind == ITEM_KIND_ARMOR) {
            var_a0 = g_CursorRenderMetadataWindows[1].text;
        }
        var_a0_2 = Draw_MeasureTextWidth(var_a0) + 0x14;
    } else {
        Draw_OffsetCursor(2, 2);
        Draw_AllocSprite(0x47);
        Draw_OffsetCursor(0x3C, 8);
        Draw_PrintRawText(Save_GetActiveMetadataBuffer());
        var_a0_2 = Draw_MeasureTextWidth(Save_GetActiveMetadataBuffer());
    }
    Draw_OffsetCursor(var_a0_2, 0xC);
    temp_v0 = Save_GetMetadataRemainingChars();
    var_s0 = temp_v0 - 1;
    if (temp_v0 != 0) {
        do {
            Draw_EmitWipeBarRect(9, 4, 0);
            Draw_OffsetCursor(0xB, 0);
            var_s0 -= 1;
        } while (var_s0 != -1);
    }
}
