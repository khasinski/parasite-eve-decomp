/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_widget.h"

void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node);

void MenuWidget_DestroyPopupNode(MenuWidgetNode *node) {
    MenuWidgetNode *owner = (MenuWidgetNode *)node->grid_width;

    owner->popup_node = 0;
    MenuWidget_DestroyNodeRecursive(node);
}
