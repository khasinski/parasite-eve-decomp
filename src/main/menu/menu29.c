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
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u32 w[6];
} InvWeaponStateCopy;

typedef struct {
    u32 w[2];
} InvArmorStateCopy;

extern u32 D_800A1AA0[];
extern int D_8009D018;
extern void *D_8009D1E0;
extern struct { char _[16]; } D_8009D254_o __asm__("D_8009D254");
#define D_8009D254 (*(void **)&D_8009D254_o)
extern s16 D_800C0E48[];
extern s8 D_800C0E20[];
extern s8 D_800C0E22[];

int Inv_CheckSlotUsable(int data);
int Inv_FindIndexByData(int data);
void MenuWidget_InitPool(void);
int Menu_GetBattleEquipMode(void);
void Battle_DispatchSpecialAction(int action);
void Inv_BuildWeaponList(int arg0, void *arg1);
void Inv_BuildArmorList(void *arg0);
void Inv_SelectActiveList(int useOverride);
void BattleCmd_LoadWeaponModifiers(void);

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

void Inv_SetActiveList(int mode, void *arg1) {
    register void *arg1_reg asm("$7");
    register void *core asm("$6");
    register void *entity_ptr asm("$3");
    register void *raw_core asm("$3");
    register u32 core_mask asm("$2");
    int result;
    int item_type;
    register u32 *src asm("$2");
    u32 *dst;
    register u32 *weapon_state asm("$5");
    void *item;
    u32 word;
    register u32 clear_low asm("$4");

    arg1_reg = arg1;
    entity_ptr = D_8009D254;
    core = 0;
    if (entity_ptr != 0) {
        raw_core = *(void **)entity_ptr;
        core_mask = raw_core != 0;
        core_mask = -core_mask;
        core = (void *)((u32)raw_core & core_mask);
    }

    switch (mode) {
    case 0:
        g_MenuCommandResult = *(int *)arg1_reg + 3;
        break;

    case 1:
        g_MenuCommandResult = *(int *)arg1_reg + 0x183;
        break;

    case 2:
        if (core != 0) {
            dst = (u32 *)D_8009D1E0;
            if (dst != 0) {
                src = *(u32 **)((char *)core + 0x68);
                *(InvWeaponStateCopy *)dst = *(InvWeaponStateCopy *)src;
                Inv_BuildWeaponList(0, D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode() != 0) {
                g_MenuCommandResult = 0x197;
            } else {
                Battle_DispatchSpecialAction(0x197);
            }
        }
        break;

    case 3:
        if (core != 0) {
            dst = (u32 *)D_8009D1E0;
            if (dst != 0) {
                src = *(u32 **)((char *)core + 0x6C);
                *(InvArmorStateCopy *)dst = *(InvArmorStateCopy *)src;
                Inv_BuildArmorList(D_8009D1E0);
            }
            if (Menu_GetBattleEquipMode() != 0) {
                g_MenuCommandResult = 0x198;
            } else {
                Battle_DispatchSpecialAction(0x198);
            }
        }
        break;

    case 5:
        if (core != 0) {
            clear_low = -0x400;
            weapon_state = *(u32 **)((char *)core + 0x68);
            item = *(void **)arg1_reg;
            word = weapon_state[3];
            word &= clear_low;
            word |= *(u16 *)((char *)item + 0xA) & 0x3FF;
            weapon_state[3] = word;

            item_type = *(u8 *)((char *)item + 6);
            if (item_type != 0 && item_type < 8) {
                result = item_type - 4;
                if (result <= 0) {
                    result = 1;
                }
            } else if (item_type < 0x13) {
                result = 0;
            } else {
                result = item_type - 0x12;
            }

            word = weapon_state[3];
            word &= 0xFFCFFFFF;
            word |= (result & 3) << 20;
            weapon_state[3] = word;
        }
        break;

    case 8:
        g_MenuCommandResult = 0x199;
        break;

    case 9:
        g_MenuCommandResult = -1;
        break;

    case 10:
        g_MenuCommandResult = 1000;
        break;

    case 11:
        g_MenuCommandResult = 1;
        break;

    case 12:
        D_8009D018 = 4;
        Inv_SelectActiveList(0);
        BattleCmd_LoadWeaponModifiers();
        g_MenuCommandResult = 2;
        break;
    }

    if (g_MenuCommandResult != 0) {
        MenuWidget_InitPool();
    }
}

int Menu_GetCommandResult(void) {
    return g_MenuCommandResult;
}

void Menu_ClearCommandResult(void) {
    g_MenuCommandResult = 0;
}
