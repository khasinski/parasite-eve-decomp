#include "pe1/menu_widget.h"
#include "pe1/inventory.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int D_8009CF24, D_8009CF28, D_8009CFBC, D_8009CF18;
extern int D_8009CF2C, D_8009CF0C, D_8009CF14;
extern int D_800A1888[], D_800A188C[], D_800A1890[], D_800A1894[];

extern MenuWidgetNode *MenuWidget_GetCurrentNode(void);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *node);
extern int Inv_RestoreSelection(int list);
extern int Inv_CheckItemEquippable(int list, int slot);
extern int Inv_GetActiveListItemType(int item);
extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
extern MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
extern void Menu_CreateItemList(void);
extern void Menu_CreateNotificationDialog(int text, int arg1);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayErrorSound(void);
extern void Menu_DrawEquipScreenHeader(void);
extern void Menu_InventoryItemHandler(void);
extern void Menu_DrawScreenModeList(void);
extern void Menu_GetStatBaseValue(void);
extern void Menu_DrawItemActionSubmenu(void);
extern void Menu_StepSkillSelect(void);
extern void Menu_DrawActionOptionList(void);

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
            equip_child->field_8C = (int)Menu_GetStatBaseValue;
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
