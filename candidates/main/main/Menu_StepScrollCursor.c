/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_widget.h"

typedef unsigned int u32;

extern int D_8009D0E8;
extern MenuWidgetNode *D_8009D15C;

int Draw_RemapStatusFlags(void);
void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);
int Menu_StepListNavigate(MenuWidgetNode *node, int buttons);

typedef int (*MenuSelectCallback)(int from_base, int from_index, int to_base, int to_index);
typedef void (*MenuCancelCallback)(void);

static int MenuWidget_FocusedCell(MenuWidgetNode *node) {
    if (node != 0 && node->cursor_x >= 0 && node->cursor_y >= 0) {
        return node->grid_width * node->cursor_y + node->cursor_x;
    }
    return -1;
}

static int MenuWidget_TargetCell(MenuWidgetNode *node) {
    if (node != 0 && node->target_x >= 0 && node->target_y >= 0) {
        return node->grid_width * node->target_y + node->target_x;
    }
    return -1;
}

static int MenuWidget_RemappedAny(int mask) {
    if (D_8009D0E8 != 0) {
        return (Draw_RemapStatusFlags() & mask) != 0;
    }
    return 0;
}

static int MenuWidget_ClampScrollToCursor(MenuWidgetNode *node) {
    int old_scroll;
    int next_scroll;
    int max_scroll;

    old_scroll = node->scroll_y;
    next_scroll = old_scroll;

    if (node->cursor_y < next_scroll) {
        next_scroll = node->cursor_y;
    } else if (node->cursor_y >= next_scroll + node->visible_rows) {
        next_scroll = node->cursor_y - node->visible_rows + 1;
    }

    if (next_scroll < 0) {
        next_scroll = 0;
    }

    max_scroll = node->y_limit - node->visible_rows;
    if (max_scroll < 0) {
        max_scroll = 0;
    }
    if (next_scroll > max_scroll) {
        next_scroll = max_scroll;
    }

    node->scroll_y = next_scroll;
    return old_scroll != next_scroll;
}

static void MenuWidget_StartScrollEase(MenuWidgetNode *node, int direction) {
    int delta;

    delta = node->field_40;
    if (direction < 0) {
        delta = -delta;
    }
    node->field_60 = (delta + ((u32)delta >> 31)) >> 1;
}

static int MenuWidget_MoveUp(MenuWidgetNode *node) {
    int moved;

    moved = 0;
    if (node->cursor_y > 0) {
        node->cursor_y--;
        Menu_PlayMoveSound();
        moved = 1;
    } else if ((node->field_64 & 0x10) != 0) {
        node->cursor_y = node->y_limit - 1;
        Menu_PlayMoveSound();
        moved = 1;
    }

    if (MenuWidget_ClampScrollToCursor(node) != 0) {
        MenuWidget_StartScrollEase(node, -1);
    }

    if (((node->field_64 >> 2) & 1) == 0) {
        moved = 1;
    }
    return moved;
}

static int MenuWidget_MoveDown(MenuWidgetNode *node) {
    int moved;
    int tail_reserve;

    moved = 0;
    tail_reserve = 0;
    if (node->cursor_x != 0 && node->field_68 != 0) {
        tail_reserve = 1;
    }

    if (node->cursor_y < node->y_limit - (tail_reserve + 1)) {
        node->cursor_y++;
        Menu_PlayMoveSound();
        moved = 1;
    } else if ((node->field_64 & 0x10) != 0) {
        node->cursor_y = 0;
        Menu_PlayMoveSound();
        moved = 1;
    }

    if (MenuWidget_ClampScrollToCursor(node) != 0) {
        MenuWidget_StartScrollEase(node, 1);
    }

    if (((node->field_64 >> 3) & 1) == 0) {
        moved = 1;
    }
    return moved;
}

static void MenuWidget_FocusNeighbor(MenuWidgetNode *current, MenuWidgetNode *next, int enter_from_bottom) {
    int target_y;
    int from_relative_y;
    int tail_reserve;

    current->cursor_x = -1;
    if (enter_from_bottom != 0) {
        next->cursor_x = next->grid_width - 1;
    } else {
        next->cursor_x = 0;
    }

    target_y = next->y_limit - 1;
    from_relative_y = current->cursor_y - current->scroll_y + next->scroll_y;
    if (from_relative_y < target_y) {
        target_y = from_relative_y;
    }
    next->cursor_y = target_y;

    tail_reserve = 0;
    if (next->field_68 != 0 && target_y == next->y_limit - 1) {
        tail_reserve = 1;
    }
    if (enter_from_bottom != 0) {
        next->cursor_x = next->grid_width - (tail_reserve + 1);
    }

    D_8009D15C = next;
    Menu_PlayMoveSound();
}

static int MenuWidget_MoveLeft(MenuWidgetNode *node) {
    if (node->cursor_x > 0) {
        if (node->cursor_x == 6) {
            node->cursor_x = 4;
        } else {
            node->cursor_x--;
        }
        Menu_PlayMoveSound();
        return 1;
    }

    if (node->field_78 != 0) {
        MenuWidget_FocusNeighbor(node, node->field_78, 1);
        return 1;
    }

    if ((node->field_64 & 1) != 0) {
        return MenuWidget_RemappedAny(0x5000);
    }
    return 1;
}

static int MenuWidget_MoveRight(MenuWidgetNode *node) {
    int tail_reserve;

    if (node->cursor_x >= 0) {
        tail_reserve = 0;
        if (node->cursor_y == node->y_limit - 1 && node->field_68 != 0) {
            tail_reserve = 1;
        }

        if (node->cursor_x < node->x_limit - (tail_reserve + 1)) {
            if (node->cursor_x == 4) {
                node->cursor_x = 6;
            } else {
                node->cursor_x++;
            }
            Menu_PlayMoveSound();
            return 1;
        }
    }

    if (node->field_7C != 0) {
        MenuWidget_FocusNeighbor(node, node->field_7C, 0);
        return 1;
    }

    if ((node->field_64 & 2) != 0) {
        return MenuWidget_RemappedAny(0x5000);
    }
    return 1;
}

static int MenuWidget_TrySelect(MenuWidgetNode *from, MenuWidgetNode *to) {
    MenuSelectCallback cb;
    int from_cell;
    int to_cell;

    if (to == 0 || to->target_x < 0) {
        return 0;
    }

    from_cell = MenuWidget_FocusedCell(from);
    to_cell = MenuWidget_TargetCell(to);
    if (from->selected_base == to->selected_base && from_cell == to_cell) {
        return 0;
    }

    cb = (MenuSelectCallback)from->field_84;
    if (cb != 0 && cb(from->selected_base, from_cell, to->selected_base, to_cell) != 0) {
        to->target_x = -1;
        Menu_PlayConfirmSound();
    } else {
        Menu_PlayErrorSound();
    }
    return 1;
}

static int MenuWidget_HandleConfirm(MenuWidgetNode *node) {
    MenuSelectCallback cb;
    int cell;
    int handled;

    if (node->field_84 == 0) {
        return 0;
    }

    handled = 0;
    if (node->target_x >= 0) {
        cell = MenuWidget_FocusedCell(node);
        if (cell != MenuWidget_TargetCell(node)) {
            cb = (MenuSelectCallback)node->field_84;
            if (cb(node->selected_base, cell, node->selected_base, cell) != 0) {
                node->target_x = -1;
                Menu_PlayConfirmSound();
            } else {
                Menu_PlayErrorSound();
            }
            handled = 1;
        }
    }

    handled |= MenuWidget_TrySelect(node, node->field_78);
    handled |= MenuWidget_TrySelect(node, node->field_7C);
    return handled;
}

static void MenuWidget_RestoreTarget(MenuWidgetNode *node) {
    node->cursor_x = node->target_x;
    node->cursor_y = node->target_y;
    node->target_x = -1;
    if (node->cursor_y < node->scroll_y) {
        node->scroll_y = node->cursor_y;
    } else if (node->cursor_y >= node->scroll_y + node->visible_rows) {
        node->scroll_y = node->cursor_y - node->visible_rows + 1;
    }
    if (node->field_88 != 0) {
        ((MenuCancelCallback)node->field_88)();
    }
    Menu_PlayCancelSound();
}

static int MenuWidget_RestoreNeighbor(MenuWidgetNode *current, MenuWidgetNode *node) {
    if (node == 0 || node->target_x < 0) {
        return 0;
    }

    node->cursor_x = node->target_x;
    node->cursor_y = node->target_y;
    node->target_x = -1;
    current->cursor_x = -1;
    MenuWidget_ClampScrollToCursor(node);
    D_8009D15C = node;
    return 1;
}

static int MenuWidget_HandleCancel(MenuWidgetNode *node) {
    int handled;

    handled = 0;
    if (node->target_x >= 0) {
        MenuWidget_RestoreTarget(node);
        handled = 1;
    }

    handled |= MenuWidget_RestoreNeighbor(node, node->field_78);
    handled |= MenuWidget_RestoreNeighbor(node, node->field_7C);
    return handled;
}

static int MenuWidget_HandlePagedChild(MenuWidgetNode *node, u32 buttons) {
    int next_y;
    int tail_reserve;

    if (node->field_80 == 0 || D_8009D15C != node) {
        return 0;
    }

    if ((buttons & 4) != 0) {
        next_y = node->cursor_y - node->visible_rows;
        if (next_y < 0) {
            next_y = 0;
        }
        node->cursor_y = next_y;
        return Menu_StepListNavigate((MenuWidgetNode *)node->field_80, 0x1000);
    }

    if ((buttons & 8) != 0) {
        tail_reserve = 0;
        if (node->cursor_x == 1 && node->field_68 != 0) {
            tail_reserve = 1;
        }
        next_y = node->cursor_y + node->visible_rows;
        if (node->y_limit - (tail_reserve + 1) < next_y) {
            next_y = node->y_limit - (tail_reserve + 1);
        }
        node->cursor_y = next_y;
        return Menu_StepListNavigate((MenuWidgetNode *)node->field_80, 0x4000);
    }

    return 0;
}

int Menu_StepScrollCursor(MenuWidgetNode *node, u32 buttons) {
    int moved;

    moved = 0;
    if (D_8009D0E8 != 0 && (Draw_RemapStatusFlags() & 0x20) != 0) {
        return 1;
    }

    if ((buttons & 0x1000) != 0) {
        return MenuWidget_MoveUp(node);
    }

    if ((buttons & 0x4000) != 0) {
        return MenuWidget_MoveDown(node);
    }

    if ((buttons & 0x8000) != 0) {
        return MenuWidget_MoveLeft(node);
    }

    if ((buttons & 0x2000) != 0) {
        return MenuWidget_MoveRight(node);
    }

    if ((buttons & 0x10000) != 0) {
        return MenuWidget_HandleConfirm(node);
    }

    if ((buttons & 0x40) != 0) {
        moved = MenuWidget_HandleCancel(node);
        return moved;
    }

    if (MenuWidget_HandlePagedChild(node, buttons) != 0) {
        return moved;
    }

    if ((buttons & 0x20) != 0) {
        return 1;
    }

    return moved;
}
