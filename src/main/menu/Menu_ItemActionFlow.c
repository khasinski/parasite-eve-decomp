/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/inventory.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/menu_widget.h"
#include "pe1/menu_equipment.h"
#include "pe1/text.h"
#include "pe1/inventory_slots.h"
#include "pe1/draw_state.h"

/* Item action submenu, skill/stat point use, the equip stats panel and the
 * item detail and item list views of the inventory menu. Contiguous at
 * 0x8004778C and joined by private data and helper calls. */

int Menu_EquipStatsInputHandler(MenuWidgetNode *node, int flags);
int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1);
int g_MenuSelectedItemIndex;
int D_8009CFBC;
int MenuWidget_GetCellIndex(int arg0);
void Menu_CreateItemActionSubmenu(void);
void Menu_CreateItemList(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
void Menu_PlayCancelSound(void);
int g_MenuActionTextBase;
void Menu_DrawItemActionSubmenu(void);
int g_InvItemUsableFlag;
int g_MenuSelectedItemList;
int g_MenuSelectedItemSlot;
void Draw_OffsetCursor(int arg0, int arg1);
void Draw_StatePush(void);
void Draw_AllocSprite(int arg0);
void Draw_StatePop(void);
void Sfx_DrawActiveListSlot(int arg0);
#define NULL ((void *)0)
s32 MenuWidget_GridCellIndex();
s32 func_80052F0C();
s32 Inv_RestoreSelection();
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
void Menu_DrawEquipStats(void);
void Menu_DrawItemListInvPanel(void);
extern void Menu_DrawEquipStatsDelta(ItemDataRecord *data);
extern void Draw_PrintNumberWidth4Unk(int arg0);
extern void Draw_PrintSignedNumberWidth4(int arg0);
/* Inlined into Menu_OpenItemList, like Menu_CreateItemActionSubmenu; no
 * out-of-line copy exists in retail. */
static inline void Menu_CreateScreenModeSubmenu(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x2c, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x2c, parent, parent);
    parent->draw = Menu_DrawEquipScreenHeader;
    parent->update = Menu_InventoryItemHandler;
    child->draw = Menu_DrawScreenModeList;
    child->selectionAvailable = Menu_GetStatBaseValue;
    if (D_8009CF0C != 0) {
        parent->visible_rows += 0x10;
        child->visible_rows = 2;
        child->y_limit = 2;
        child->x += 0x1c;
    }
    MenuWidget_SetCurrentNode(child);
}

void Menu_OpenItemList(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_PlayMoveSound(void);
void Inv_BuildStorageDisplay(void);
void MenuWidget_NavScrollTo(s32 selected_base);
extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *arg1, int arg2, int arg3);
extern void Menu_DrawUsableItemActionList2(void);
extern void Inv_SelectActiveList(int arg0);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void Inv_InitWayneStorage(void);
extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *arg0, int arg1);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Menu_DrawItemListHeader(void);
int D_8009CFC0;

int Menu_InventoryItemHandler(MenuWidgetNode *arg0, int arg1) {
    int temp;

    if (arg1 & 0x10000) {
        temp = MenuWidget_GetCellIndex(MenuWidget_GetChild(arg0, 0));
        g_MenuSelectedItemIndex = temp;
        if (temp >= 0) {
            if ((temp & 1) || (D_8009CFBC == 0)) {
                Menu_CreateItemActionSubmenu();
            } else {
                Menu_CreateItemList();
            }
            Menu_PlayConfirmSound();
        } else {
            Menu_PlayErrorSound();
        }
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }

    return 1;
}

/* Also inlined into Menu_OpenItemList, which follows in this unit. */
inline void Menu_CreateItemActionSubmenu(void) {
    void *node;
    void *child;

    node = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(4, node, node);
    *(void **)((char *)node + 0x30) = Menu_DrawItemActionSubmenu;
    *(void **)((char *)node + 0x2C) = Menu_StepSkillSelect;
    *(void **)((char *)child + 0x30) = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuSelectedItemIndex & 1) {
        *(int *)((char *)node + 0x38) -= 0x1C;
        *(int *)((char *)child + 0x1C) -= 0x1C;
    }
}

void Menu_DrawItemActionSubmenu(void) {
    int selected;
    int icon;
    u8 *entry;

    selected = Inv_RestoreSelection(g_MenuSelectedItemList);
    Draw_OffsetCursor(4, 4);
    Draw_StatePush();

    if (g_MenuSelectedItemSlot >= 0) {
        entry = Inv_LookupActiveListData(selected);
        entry += g_MenuSelectedItemSlot;
        icon = entry[0x15] & 0x1F;
        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(icon + 0x22);
        } else {
            Draw_AllocSprite(icon + 0x36);
        }
        Draw_OffsetCursor(0x24, 0);
    } else {
        Draw_PrintTextById(0x66);
        Draw_OffsetCursor(Draw_MeasureTextWidth(Str_LookupTable4(0x66)), 0);
    }

    Draw_PrintTextById((g_MenuSelectedItemIndex & 1) | 0x64);
    Draw_StatePop();

    if ((g_MenuSelectedItemIndex & 1) == 0) {
        Draw_OffsetCursor(0, 0xE);
        Sfx_DrawActiveListSlot(selected);
        Draw_OffsetCursor(0, 0xE);
        Draw_PrintTextById(0x67);
    }

    Draw_OffsetCursor(0, 0x14);
    Draw_PrintTextById(0x68);
}

int Menu_StepSkillSelect(MenuWidgetNode *arg0, int arg1) {
    M2C_UNK var_a0;
    s32 *temp_a1;
    s32 pad_[1];
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s32 var_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (temp_v0 != 0) {
            if (temp_v0 == 1) {
                goto shared_1;
            }
            return 1;
        }
        {
            MenuWidget_DestroyNode(arg0);
            Inv_TransferItemBetweenLists(g_MenuSelectedItemList, g_MenuSelectedItemSlot, g_MenuSelectedItemIndex, g_StatBaseTable[g_MenuSelectedItemIndex]);
            temp_a1 = &g_StatBaseTable[g_MenuSelectedItemIndex];
            temp_v0_2 = *temp_a1;
            *temp_a1 = temp_v0_2 - (temp_v0_2 < 0x3E7);
            ((void (*)())MenuWidget_NavScrollTo)(0x2C, temp_a1);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xA);
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (var_v0 == 0) {
                var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            }
            MenuWidget_SetCurrentNode(var_v0);
            var_s1 = 0;
            Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
            do {
                temp_s0 = Inv_RestoreSelection(var_s1);
                if ((func_80052F0C() == 0) && ((var_a0 = 2, (temp_s0 == D_800C0E20.tracked[0])) || (var_a0 = 3, (temp_s0 == D_800C0E20.tracked[2])))) {
                    Inv_SetActiveList(var_a0, 0);
                }
                var_s1 += 1;
            } while (var_s1 < 2);
            Menu_PlayConfirmSound();
            return 1;
        }
    } else {
        if (arg1 & 0x40) {
shared_1:
            MenuWidget_DestroyNode(arg0);
            Menu_PlayCancelSound();
        }
        return 1;
    }
}

void Menu_CreateEquipStatsPanel(int arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(0xA, arg0, 0, 0);
    node->draw = Menu_DrawEquipStats;
    node->update = Menu_EquipStatsInputHandler;

    child = MenuWidget_CreateNode(0x1C, node, node);
    child->draw = Menu_DrawItemListInvPanel;
    child->cursor_x = -1;
}

void Menu_DrawEquipStats(void) {
    int value;
    ItemDataRecord *data;

    value = Inv_RestoreSelection(1);
    data = Inv_LookupActiveListData(value);

    Draw_OffsetCursor(4, 4);
    Sfx_DrawActiveListSlot(value);
    Draw_OffsetCursor(-4, -4);
    Menu_DrawEquipStatsDelta(data);

    if (data != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(data->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[2]);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[1]);

        Draw_OffsetCursor(-0x2D, -0xE);
        Draw_PrintNumberWidth4Unk(data->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(data->bonusStats[0]);

        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }
}

int Menu_EquipStatsInputHandler(MenuWidgetNode *node, int flags) {
    void *selected;
    void *child;
    void *target;

    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x4000) {
        M2C_FIELD(child, s32 *, 0x44) = -1;
        child = MenuWidget_FindByModeAndSelectedBase(2, 0xB);
        selected = child;
        M2C_FIELD(selected, s32 *, 0x44) = 0;
        M2C_FIELD(selected, s32 *, 0x48) = 0;
        MenuWidget_SetCurrentNode(selected);
        Menu_PlayMoveSound();
        return 1;
    }
    if (flags & 0x10000) {
        Menu_OpenItemList();
        return 1;
    }
    if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);
        target = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (target == NULL) {
            target = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(target);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_CreateItemDetailView(MenuWidgetNode *arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *parent;
    MenuWidgetNode *link;

    parent = MenuWidget_CreateSimpleNode(0xB, arg0, 0, 0);
    node = MenuWidget_CreateNode(0xB, parent, parent);
    parent->update = Menu_DetailViewInput;
    node->draw = Menu_DrawUsableItemActionList2;

    Inv_SelectActiveList(0);
    Draw_SetPrimCallback(node, ((char *)Inv_LookupActiveListData(Inv_RestoreSelection(1)))[0x14]);
    MenuWidget_ClearColumnLayout(node);

    link = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (link != 0) {
        node->linkedPrevious = link;
        link->linkedNext = node;
    }

    Inv_InitWayneStorage();
}

int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1) {
    MenuWidgetNode *node;

    node = MenuWidget_GetChild(arg0, 0);
    Menu_InventoryNavigate(node, MenuWidget_FindByModeAndSelectedBase(2, 6), arg1);

    if (arg1 & 0x1000) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    }

    return 1;
}

int Menu_InventoryNavigate(MenuWidgetNode *arg0, MenuWidgetNode *arg1, unsigned int flags) {
    int ret;
    MenuWidgetNode *node;

    ret = 0;
    if (flags & 0x10000) {
        Menu_OpenItemList();
        ret = 1;
    } else if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);

        node = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(node);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
        Menu_PlayCancelSound();
        ret = 1;
    }

    return ret;
}

void Menu_CreateItemList(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    ItemDataRecord *data;
    int i;
    int value;

    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->draw = Menu_DrawItemListHeader;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);

    Menu_SetDeferredCallback(0);

    parent->grid_width = 0xDC;
    parent->x = 0x32;
    parent->visible_rows += 0xA;

    child->x = parent->grid_width - 0x44;
    child->y += 8;

    D_8009CFC0 = 0x37;

    if (g_InvTrackedSlots[2] >= 0) {
        Inv_SelectActiveList(0);
        data = Inv_LookupActiveListData(g_InvTrackedSlots[2]);

        for (i = 0; i < data->tailCount; i++) {
            if ((data->tailData[i] & 0xE0) == 0xA0) {
                break;
            }
        }

        value = D_8009CFC0 - 1;
        D_8009CFC0 = value + (data->tailData[i] & 0x1F);
    }
}

void Menu_DrawItemListHeader(void) {
    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(D_8009CFC0);
    Draw_OffsetCursor(0x20, 0);
    Draw_PrintTextById(0x1C);
    Draw_OffsetCursor(-0x20, 0x10);
    Draw_PrintTextById(0x1D);
}

void Menu_OpenItemList(void)
{
    int list = 0;
    int slot = -1;
    int status;
    int kind;
    int i;
    MenuWidgetNode *node;
    ItemDataRecord *item;

    node = MenuWidget_GetCurrentNode();
    switch (node->selected_base) {
    case 6:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
    case 27:
        list = 0;
        break;
    case 11:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
        list = 1;
        break;
    case 28:
        list = 1;
        break;
    }

    item = Inv_LookupActiveListData(Inv_RestoreSelection(list));
    /* slot == -1 selects tailCount; add the offset before forming a pointer. */
    if (!*(u8 *)((unsigned int)item + slot + 0x15)) goto error;

    D_8009CF24 = list;
    D_8009CF28 = slot;
    status = Inv_CheckItemEquippable(list, slot);
    D_8009CFBC = status == 5;
    if (status == 1 || status == 2 || status == 5) {
        kind = Inv_GetActiveListItemType(Inv_RestoreSelection(list));
        D_8009CF18 = kind != 9;
        i = (D_800A1888[0] > 0) + (D_800A188C[0] > 0) + (D_800A1890[0] > 0);
        if (D_800A1894[0] > 0 ? i + 1 >= 2 : i >= 2) {
            Menu_CreateScreenModeSubmenu();
            goto confirm;
        }

        for (i = 0; i < 4 && D_800A1888[i] == 0; ++i) {}
        D_8009CF2C = i < 4 ? i : 0;
        item = Inv_LookupActiveListData(Inv_RestoreSelection(list));
        kind = *(u8 *)((unsigned int)item + slot + 0x15) & 0x1f;
        if (status == 1 || (!(D_8009CF2C & 1) && status == 2) ||
            ((D_8009CF2C & 1) && status == 5 && (unsigned)(kind - 8) >= 3)) {
            Menu_CreateItemActionSubmenu();
        } else {
            Menu_CreateItemList();
        }
    confirm:
        Menu_PlayConfirmSound();
        return;
    }
    switch (status) {
    case 0: Menu_CreateItemList(); return;
    case 4: kind = 0x63; break;
    default: kind = 7; break;
    }
    Menu_CreateNotificationDialog(kind, 0);
    return;
error:
    Menu_PlayErrorSound();
}
