/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_equipment.h"
#include "pe1/text.h"
#include "pe1/draw_state.h"

void Menu_DrawItemListHeader(void);

int D_8009CFC0;

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
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
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
            MenuWidgetNode *equip_child;
            MenuWidgetNode *raw_child;
            MenuWidgetNode *raw_parent;
            register int create_mode asm("$4");
            raw_parent = MenuWidget_CreateSimpleNode(0x2c, MenuWidget_GetCurrentNode(), 0, 1);
            create_mode = 0x2c;
            asm volatile("" : "=r"(raw_parent) : "0"(raw_parent), "r"(create_mode));
            parent = raw_parent;
            raw_child = MenuWidget_CreateNode(create_mode, parent, parent);
            {
                void (*callback)();
                callback = Menu_DrawEquipScreenHeader;
                parent->draw = callback;
                callback = Menu_InventoryItemHandler;
                parent->update = callback;
            }
            equip_child = raw_child;
            equip_child->draw = Menu_DrawScreenModeList;
            equip_child->selectionAvailable = Menu_GetStatBaseValue;
            if (D_8009CF0C != 0) {
                parent->visible_rows += 0x10;
                equip_child->visible_rows = 2;
                equip_child->y_limit = 2;
                equip_child->x += 0x1c;
            }
            MenuWidget_SetCurrentNode(equip_child);
            goto confirm;
        }

        for (i = 0; i < 4 && D_800A1888[i] == 0; ++i) {}
        D_8009CF2C = i < 4 ? i : 0;
        item = Inv_LookupActiveListData(Inv_RestoreSelection(list));
        kind = *(u8 *)((unsigned int)item + slot + 0x15) & 0x1f;
        if (status == 1 || (!(D_8009CF2C & 1) && status == 2) ||
            ((D_8009CF2C & 1) && status == 5 && (unsigned)(kind - 8) >= 3)) {
            MenuWidgetNode *raw_parent2;
            register int create_mode2 asm("$4");
            raw_parent2 = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
            create_mode2 = 4;
            asm volatile("" : "=r"(raw_parent2) : "0"(raw_parent2), "r"(create_mode2));
            parent = raw_parent2;
            child = MenuWidget_CreateNode(create_mode2, parent, parent);
            parent->draw = Menu_DrawItemActionSubmenu;
            parent->update = Menu_StepSkillSelect;
            child->draw = Menu_DrawActionOptionList;
            D_8009CF14 = 5;
            MenuWidget_SetCurrentNode(child);
            if (D_8009CF2C & 1) {
                parent->visible_rows -= 0x1c;
                child->y -= 0x1c;
            }
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
