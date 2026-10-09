#include "common.h"
#include "pe1/save.h"
#include "pe1/game_state.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern u8 D_80092354[];
extern u8 D_80092380[];
extern u8 D_800923A0[];
void Menu_DrawSaveMetadataPreview(void);
int Menu_StepNameEntry(MenuWidgetNode *parent, unsigned int flags);
int Menu_StepNameEntryAlt(MenuWidgetNode *parent, unsigned int flags);
void Menu_DrawItemIconList(int arg0);
void Menu_DrawMemCardSlot1List(int arg0);
void Menu_DrawMemCardSlot2List(int arg0);
void Menu_DrawMemCardSlot3List(int arg0);

void Menu_OpenRenameScreen(s32 arg0) {
    u8 *var_v1;
    s32 temp_v0_5;
    ItemDataRecord *record;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_3;
    MenuWidgetNode *temp_v0_4;
    MenuWidgetNode *var_a3;

    temp_v0 = MenuWidget_CreateSimpleNode(0x17, 0, 0, 0);
    (temp_v0)->update = (void (*)())Menu_StepNameEntryAlt;
    temp_v0_5 = Save_GetMetadataWindowIndex();
    var_v1 = D_80092354;
    if (temp_v0_5 != 0) {
        var_v1 = D_80092380;
    }
    temp_v0->appearance.gradientPoints = var_v1;
    if (Save_GetMetadataWindowIndex() != 0) {
        var_a3 = MenuWidget_CreateNode(0x18, temp_v0, temp_v0);
        (var_a3)->draw = &Menu_DrawMemCardSlot2List;
        (var_a3)->y = ((var_a3)->y - 0x34);
        var_a3 = MenuWidget_CreateNode(0x19, temp_v0, temp_v0);
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
        temp_v0_4->appearance.gradientPoints = D_800923A0;
    }
    Save_SelectMetadataWindow(g_MenuRenameTargetRecord);
    Save_LoadMetadataWindowText();
    Inv_ClearEquipFlagForKind(g_MenuRenameTargetRecord);
    g_GameState.flags |= 0x8000;
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
    u8 *text;
    s32 remaining;
    s32 lastSlot;
    s32 frame_pad[2];

    (void) frame_pad;
    if (g_MenuRenameTargetRecord != NULL) {
        Draw_OffsetCursor(0x1C, 0xC);
        Sfx_CursorRenderData(g_MenuRenameTargetRecord);
        text = g_CursorRenderMetadataWindows[0].text;
        if (g_MenuRenameTargetRecord->kind == ITEM_KIND_ARMOR) {
            text = g_CursorRenderMetadataWindows[1].text;
        }
        Draw_OffsetCursor(Draw_MeasureTextWidth(text) + 0x14, 0xC);
    } else {
        Draw_OffsetCursor(2, 2);
        Draw_AllocSprite(0x47);
        Draw_OffsetCursor(0x3C, 8);
        Draw_PrintRawText(Save_GetActiveMetadataBuffer());
        Draw_OffsetCursor(Draw_MeasureTextWidth(Save_GetActiveMetadataBuffer()), 0xC);
    }
    remaining = Save_GetMetadataRemainingChars();
    lastSlot = remaining - 1;
    if (remaining != 0) {
        do {
            Draw_EmitWipeBarRect(9, 4, 0);
            Draw_OffsetCursor(0xB, 0);
            lastSlot -= 1;
        } while (lastSlot != -1);
    }
}
