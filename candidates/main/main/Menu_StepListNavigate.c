/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned int u32;

typedef struct MenuListNavigateRoot {
    char pad_00[0x34];
    struct MenuListNavigateChild *child;
    char pad_38[0x08];
    int flags;
} MenuListNavigateRoot;

typedef struct MenuListNavigateChild {
    char pad_00[0x38];
    int page_rows;
    char pad_3C[0x04];
    int row_step;
    char pad_44[0x14];
    int row_count;
    int scroll_y;
    int scroll_step;
} MenuListNavigateChild;

void Menu_PlayMoveSound(void);

int Menu_StepListNavigate(MenuListNavigateRoot *node, u32 buttons) {
    MenuListNavigateChild *list;
    int old_scroll;
    int new_scroll;
    int max_scroll;
    int delta;
    int moved;
    int handled;
    int value;

    list = node->child;
    handled = 0;

    if ((buttons & 0x1004) != 0) {
        moved = 0;
        delta = -list->page_rows;
        if (list->scroll_step != 0) {
            handled = 1;
        } else {
            old_scroll = list->scroll_y;
            new_scroll = old_scroll + delta;
            list->scroll_y = new_scroll;
            if (new_scroll < 0) {
                list->scroll_y = 0;
            } else {
                max_scroll = list->row_count - list->page_rows;
                if (max_scroll < new_scroll) {
                    list->scroll_y = max_scroll;
                }
            }

            moved = old_scroll != list->scroll_y;
            if (moved) {
                value = list->row_step;
                if (delta <= 0) {
                    value = -value;
                }
                list->scroll_step = (value + ((u32)value >> 31)) >> 1;
            }
        }
        if (moved) {
            Menu_PlayMoveSound();
            handled = 1;
        }
    } else if ((buttons & 0x4008) != 0) {
        moved = 0;
        delta = list->page_rows;
        if (list->scroll_step != 0) {
            handled = 1;
        } else {
            old_scroll = list->scroll_y;
            new_scroll = old_scroll + delta;
            list->scroll_y = new_scroll;
            if (new_scroll < 0) {
                list->scroll_y = 0;
            } else {
                max_scroll = list->row_count - list->page_rows;
                if (max_scroll < new_scroll) {
                    list->scroll_y = max_scroll;
                }
            }

            moved = old_scroll != list->scroll_y;
            if (moved) {
                value = list->row_step;
                if (delta <= 0) {
                    value = -value;
                }
                list->scroll_step = (value + ((u32)value >> 31)) >> 1;
            }
        }
        if (moved) {
            Menu_PlayMoveSound();
            handled = 1;
        }
    }

    if (handled) {
        return 1;
    }

    if ((node->flags & 0x40) != 0) {
        return 0;
    }

    return 1;
}
