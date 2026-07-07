/* CC1_FLAGS: -g3 */

#include "pe1/menu_widget.h"

extern MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *arg1, int arg2, int arg3);
extern MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *arg1, MenuWidgetNode *arg2);
extern void Menu_DetailViewInput(void);
extern void Menu_DrawUsableItemActionList2(void);
extern void Inv_SelectActiveList(int arg0);
extern int Inv_RestoreSelection(int arg0);
extern MenuWidgetNode *Inv_LookupActiveListData(int arg0);
extern void Draw_SetPrimCallback(MenuWidgetNode *node, int value);
extern void MenuWidget_ClearColumnLayout(MenuWidgetNode *node);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
extern void Inv_InitWayneStorage(void);

void Menu_CreateItemDetailView(MenuWidgetNode *arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *parent;
    MenuWidgetNode *link;

    parent = MenuWidget_CreateSimpleNode(0xB, arg0, 0, 0);
    node = MenuWidget_CreateNode(0xB, parent, parent);
    parent->update = Menu_DetailViewInput;
    node->field_30 = Menu_DrawUsableItemActionList2;

    Inv_SelectActiveList(0);
    Draw_SetPrimCallback(node, ((char *)Inv_LookupActiveListData(Inv_RestoreSelection(1)))[0x14]);
    MenuWidget_ClearColumnLayout(node);

    link = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (link != 0) {
        node->field_78 = link;
        link->field_7C = node;
    }

    Inv_InitWayneStorage();
}
