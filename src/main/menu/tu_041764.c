/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

int g_MenuActiveWidget;

int MenuWidget_GridCellIndex(int);
void Draw_OffsetCursor(int, int);
void Draw_SetTextDimmed(int);
void Draw_AllocSprite(int);

int g_MenuSelectedItemData;

void Sfx_CursorRenderData(int);

#include "include_asm.h"

#include "pe1/battle_cmd.h"

BattleCmdEntry *g_BattleCmdStackTop;
extern BattleCmdEntry g_BattleCmdStack[];

void BattleCmd_UndoPending(void);

int g_MenuCommandResult;

void Menu_DrawSaveSlot1Item(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x63);
}

void Menu_DrawSaveSlot2Item(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x66);
}

void Menu_DrawSaveSlot3Item(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x62);
}

void Menu_DrawSoundEffectItem(void) {
    Sfx_CursorRenderData(g_MenuSelectedItemData);
}

void BattleCmd_InitTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_041764", BattleCmd_AllocSlot);

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_041764", BattleCmd_UndoPending);

void BattleCmd_ResetTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

void BattleCmd_UndoAllPending(void) {
    while (g_BattleCmdStack < g_BattleCmdStackTop) {
        BattleCmd_UndoPending();
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/tu_041764", Inv_SetActiveList);

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}
