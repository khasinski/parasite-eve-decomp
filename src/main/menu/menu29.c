/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

int g_MenuActiveWidget;

int MenuWidget_GridCellIndex(int);
void Draw_OffsetCursor(int x, int y);
void Draw_SetTextDimmed(int value);
void Draw_AllocSprite(int);

void *g_MenuSelectedItemData;

void Sfx_CursorRenderData(void *record);

#include "include_asm.h"

#include "pe1/battle_cmd.h"

BattleCmdEntry *g_BattleCmdStackTop;
extern BattleCmdEntry g_BattleCmdStack[];
extern BattleCmdEntry *D_8009D014;
extern char D_800A1B30[];

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

BattleCmdEntry *BattleCmd_AllocSlot(void) {
    register BattleCmdEntry *top asm("$8");
    register BattleCmdEntry *end asm("$3");
    register BattleCmdEntry *dst asm("$7");
    register BattleCmdEntry *limit asm("$10");
    BattleCmdEntry *next;

    top = D_8009D014;
    end = (BattleCmdEntry *)D_800A1B30;
    if (top < end) {
        next = top + 1;
        D_8009D014 = next;
    } else {
        top = end - 4;
        end = end - 1;
        dst = top;
        if (top < end) {
            limit = end;
            do {
                BattleCmdEntry *src = top + 1;
                *dst = *src;
                top += 1;
                dst = top;
            } while (top < limit);
        }
    }
    return top;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu29", BattleCmd_UndoPending);

void BattleCmd_ResetTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

void BattleCmd_UndoAllPending(void) {
    while (g_BattleCmdStack < g_BattleCmdStackTop) {
        BattleCmd_UndoPending();
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu29", Inv_SetActiveList);

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}
