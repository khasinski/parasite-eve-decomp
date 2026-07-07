/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/menu_widget.h"

extern MenuWidgetNode *g_MenuActiveWidget;

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_GridCellIndex(MenuWidgetNode *node);
int Inv_RestoreSelection(unsigned int index);
int Inv_GetActiveListItemType(int index);
int Inv_IsActiveListOverrideSelected(void);
void Inv_SelectActiveList(int useOverride);
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Inv_GetPackedListCount(void);
void Draw_SetPrimCallback(MenuWidgetNode *node, int count);
void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw)(int));
void Menu_HandleInput(int index);

void Menu_DrawEquipSelectionList(MenuWidgetNode *node) {
    int mask;
    int excluded;
    int index;
    int type;

    index = MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0x36));
    if (index >= 0) {
        if (node->selected_base == 7) {
            mask = 0x33E;
            excluded = Inv_RestoreSelection(0);
            type = Inv_GetActiveListItemType(excluded);
            if (type == 0 || type >= 6) {
                mask = 1 << type;
            } else {
                mask = 0x3E;
            }

            if (Inv_IsActiveListOverrideSelected() != index) {
                excluded = -1;
            }
        } else {
            mask = 0x33E;
            excluded = -1;
        }

        Inv_SelectActiveList(index);
        Inv_BuildFilteredPackedListExcluding(mask, excluded);
        Draw_SetPrimCallback(node, Inv_GetPackedListCount());
    }

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_HandleInput);
}
