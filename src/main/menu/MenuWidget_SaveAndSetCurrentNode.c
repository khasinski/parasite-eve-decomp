/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *g_MenuWidgetCurrentNode;
MenuWidgetNode *g_MenuWidgetSavedNode;

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0) {
    g_MenuWidgetSavedNode = g_MenuWidgetCurrentNode;
    g_MenuWidgetCurrentNode = arg0;
}
