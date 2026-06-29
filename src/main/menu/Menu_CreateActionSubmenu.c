#include "pe1/menu_widget.h"

typedef unsigned int u32;

extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int arg0, MenuWidgetNode *arg1, int arg2, int arg3);
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
