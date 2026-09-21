#include "pe1/menu_inventory.h"

static inline int Move(MenuWidgetNode *node, int delta) {
    int changed = 0;

    if (!node->scroll_adjust) {
        int old = node->scroll_y;
        node->scroll_y += delta;
        if (node->scroll_y < 0) {
            node->scroll_y = 0;
        } else if (node->scroll_y > node->y_limit - node->visible_rows) {
            node->scroll_y = node->y_limit - node->visible_rows;
        }
        changed = old != node->scroll_y;
        if (changed) {
            int speed = node->disabled;
            node->scroll_adjust = (delta > 0 ? speed : -speed) / 2;
        }
    }
    return changed;
}

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags) {
    MenuWidgetNode *list = node->list;
    int handled = 0;

    if (flags & 0x1004) {
        if (Move(list, -list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    } else if (flags & 0x4008) {
        if (Move(list, list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    }
    return handled || !(node->flags & 0x40);
}
