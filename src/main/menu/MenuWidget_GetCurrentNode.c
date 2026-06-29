/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/menu_widget.h"

MenuWidgetNode *g_MenuWidgetCurrentNode;

MenuWidgetNode *MenuWidget_GetCurrentNode(void) {
    return g_MenuWidgetCurrentNode;
}
