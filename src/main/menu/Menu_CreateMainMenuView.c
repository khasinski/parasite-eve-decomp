
#include "pe1/menu_widget.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, int arg1, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
void Menu_DrawMainMenuList(void);

void Menu_CreateMainMenuView(int arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x20, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x20, parent, parent);
    parent->update = Menu_MainMenuInputHandler;
    child->field_30 = Menu_DrawMainMenuList;
    MenuWidget_SetCurrentNode(child);
    Draw_SetPrimCallback(child, 4);
}

#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *arg0, int arg1);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *arg0);
extern void MenuWidget_DestroyNode(MenuWidgetNode *arg0);
extern void Menu_CreateActionSubmenu(MenuWidgetNode *arg0);
extern void func_8004B03C(MenuWidgetNode *arg0);
extern void Menu_OpenBlendColorScreen(MenuWidgetNode *arg0);
extern void Menu_OpenScreenAdjustView(MenuWidgetNode *arg0);
extern void Inv_InitMaxLevelInventory(int arg0);
extern void Menu_CreateBonusPointAllocationView(void);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *child = MenuWidget_GetChild(arg0, 0);

    if (arg1 & 0x10000) {
        switch (MenuWidget_GridCellIndex(child)) {
        case 0:
            Menu_CreateActionSubmenu(child);
            break;
        case 1:
            func_8004B03C(child);
            break;
        case 2:
            Menu_OpenBlendColorScreen(child);
            break;
        case 3:
            Menu_OpenScreenAdjustView(child);
            break;
        case 4:
        case 5:
            Inv_InitMaxLevelInventory(MenuWidget_GridCellIndex(child) - 4);
            MenuWidget_DestroyNode(arg0);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayConfirmSound();
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
    }
    return 1;
}

#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_CreateNode(int arg0, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *arg0, int arg1);
extern int MenuWidget_GridCellIndex(MenuWidgetNode *arg0);
extern void MenuWidget_DestroyNode(MenuWidgetNode *arg0);
extern void MenuWidget_SetCurrentNode(MenuWidgetNode *arg0);
extern void Menu_PlayConfirmSound(void);
extern void Menu_PlayCancelSound(void);
extern void Menu_SetActionSubmenuSelection(int arg0);
extern void MenuWidget_SetColumnLayoutMode(int arg0);
extern void Menu_DrawActionSubmenuList(void);
extern void Menu_DrawBlendColorOptionListUnk(void);

int func_8004AFA4(MenuWidgetNode *arg0, u32 arg1);
int func_8004B0A4(MenuWidgetNode *arg0, u32 arg1);

void Menu_CreateActionSubmenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x21, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x21, created, created);
    created->update = func_8004AFA4;
    node->field_30 = Menu_DrawActionSubmenuList;
    MenuWidget_SetCurrentNode(node);
}

int func_8004AFA4(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        Menu_SetActionSubmenuSelection(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void func_8004B03C(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x23, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x23, created, created);
    created->update = func_8004B0A4;
    node->field_30 = Menu_DrawBlendColorOptionListUnk;
    MenuWidget_SetCurrentNode(node);
}

int func_8004B0A4(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        MenuWidget_SetColumnLayoutMode(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}
