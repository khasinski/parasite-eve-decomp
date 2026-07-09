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

typedef signed char s8;
typedef short s16;
typedef unsigned int u32;

extern u32 D_800A1AA0[];
extern s16 D_800C0E48[];
extern s8 D_800C0E20[];
extern s8 D_800C0E22[];

int Inv_CheckSlotUsable(int data);
int Inv_FindIndexByData(int data);

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

void BattleCmd_UndoPending(void) {
    register u32 *entry asm("$4");
    register u32 *entry_tmp asm("$2");
    register u32 *top asm("$3");
    int opcode;
    s16 *slot;
    int data;
    int index;

    top = D_8009D014;
    if (D_800A1AA0 < top) {
        entry_tmp = top - 9;
        asm volatile("" : "=r"(entry_tmp) : "0"(entry_tmp));
        opcode = top[-9];
        asm volatile("addu %0,%1,$0" : "=r"(entry) : "r"(entry_tmp));
        asm volatile("sw $2,%gp_rel(D_8009D014)($gp)");

        switch (opcode) {
        case 0:
            slot = &D_800C0E48[entry[2]];
            if (*slot == 0) {
                *slot = entry[1];
            } else {
                Inv_CheckSlotUsable(entry[1]);
            }
            break;

        case 1:
            break;

        case 2:
            D_800C0E20[0] = Inv_FindIndexByData(entry[1]);
            break;

        case 3:
            data = entry[1];
            index = -1;
            if (data != 0) {
                index = Inv_FindIndexByData(data);
            }
            D_800C0E22[0] = index;
            break;

        case 4:
            *(s16 *)(entry[1] + 0xA) = entry[5];
            *(s16 *)(entry[2] + 0xA) = entry[7];
            *(s16 *)(entry[3] + 0xA) = entry[8];
            break;
        }
    }
}

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
