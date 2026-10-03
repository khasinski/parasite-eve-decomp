/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* Candidate copy: include/pe1/menu_scroll_cursor.h inlined below. */
#include "pe1/menu_draw_list.h"

void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);
int Menu_StepListNavigate(MenuWidgetNode *list, unsigned int flags);

static inline int MenuWidget_CursorCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->cursor_x;
        if (x >= 0) {
            y = node->cursor_y;
            if (y >= 0)
                cell = node->grid_width * y + x;
        }
    }
    return cell;
}

static inline int MenuWidget_TargetCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->target_x;
        if (x >= 0 && (y = node->target_y) >= 0)
            cell = node->grid_width * y + x;
        else
            cell = -1;
    }
    return cell;
}

/* Rows past the last selectable one: column 1 of a list with a trailing
 * scroll row has one cell fewer. */
static inline int MenuWidget_EndMargin(MenuWidgetNode *node)
{
    return (node->cursor_x == 1 && node->has_scroll) + 1;
}

static inline void MenuWidget_ScrollToCursor(MenuWidgetNode *node)
{
    int y;
    int top;
    int height;

    y = node->cursor_y;
    top = node->scroll_y;
    if (y < top) {
        node->scroll_y = y;
    } else {
        height = node->visible_rows;
        if (y >= top + height)
            node->scroll_y = y - height + 1;
    }
}

static inline int MenuWidget_HeldShoulder(void)
{
    if (D_8009D0E8)
        return (Draw_RemapStatusFlags() & 0x5000) != 0;
    return 0;
}

/* Swaps the cursor cell of `node` with the marked cell of `other`. */
static inline int MenuWidget_SwapMarked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;
    int from;
    int to;

    done = 0;
    if (other != 0 && other->target_x >= 0) {
        from = MenuWidget_CursorCell(node);
        to = MenuWidget_TargetCell(other);
        if (node->selected_base != other->selected_base || from != to) {
            if (node->itemAction(node->selected_base, from, other->selected_base, to)) {
                other->target_x = -1;
                Menu_PlayConfirmSound();
                done = 1;
            } else {
                Menu_PlayErrorSound();
                done = 1;
            }
        }
    }
    return done;
}

/* Restores the marked cell of a linked list as its cursor and focuses it. */
static inline int MenuWidget_RestoreLinked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;

    done = 0;
    if (other != 0 && other->target_x >= 0) {
        other->cursor_x = other->target_x;
        other->target_x = -1;
        other->cursor_y = other->target_y;
        node->cursor_x = -1;
        MenuWidget_ScrollToCursor(other);
        g_MenuWidgetCurrentNode = other;
        done = 1;
    }
    return done;
}

/* Default update handler of the grid list widgets: moves the cursor with the
 * d-pad (crossing into the linked lists at the edges), scrolls, pages the
 * popup list, and swaps or restores marked cells. Returns nonzero when the
 * input was consumed. */
int Menu_StepScrollCursor(MenuWidgetNode *node, unsigned int buttons)
{
    MenuWidgetNode *link;
    int changed;
    int flags;
    int wrap;
    int limit;
    int row;
    int held;
    int margin;

    changed = 0;
    if (D_8009D0E8)
        flags = Draw_RemapStatusFlags();
    else
        flags = 0;
    if (flags & 0x20) {
        changed = 1;
    } else if (buttons & 0x1000) {
        if (node->cursor_y > 0) {
            node->cursor_y--;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            changed = 1;
            node->cursor_y = node->y_limit - 1;
            Menu_PlayMoveSound();
        }
        if (node->cursor_y < node->scroll_y && node->scroll_adjust == 0) {
            int old = node->scroll_y;
            int next = old - 1;

            node->scroll_y = next;
            if (next < 0)
                node->scroll_y = 0;
            else if (node->y_limit - node->visible_rows < next)
                node->scroll_y = node->y_limit - node->visible_rows;
            if (old != node->scroll_y)
                node->scroll_adjust = -node->disabled / 2;
        }
        changed |= !(node->layout_flags & 4);
    } else if (buttons & 0x4000) {
        wrap = 0;
        if (node->cursor_x)
            wrap = node->has_scroll != 0;
        margin = wrap + 1;
        if (node->cursor_y < node->y_limit - margin) {
            node->cursor_y++;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            node->cursor_y = 0;
            Menu_PlayMoveSound();
            changed = 1;
        }
        {
            int old = node->scroll_y;

            if (node->cursor_y >= old + node->visible_rows
                                      - (wrap && node->cursor_y == node->y_limit - 2)
                && node->scroll_adjust == 0) {
                int next = old + 1;

                node->scroll_y = next;
                if (next < 0)
                    node->scroll_y = 0;
                else if (node->y_limit - node->visible_rows < next)
                    node->scroll_y = node->y_limit - node->visible_rows;
                if (old != node->scroll_y)
                    node->scroll_adjust = node->disabled / 2;
            }
        }
        changed |= !(node->layout_flags & 8);
    } else if (buttons & 0x8000) {
        int x = node->cursor_x;

        if (x > 0) {
            node->cursor_x = x == 6 ? 4 : x - 1;
            Menu_PlayMoveSound();
            changed = 1;
        } else {
            link = node->linkedPrevious;
            if (link != 0) {
                int last;

                node->cursor_x = -1;
                limit = link->y_limit - 1;
                link->cursor_x = link->grid_width - 1;
                row = node->cursor_y - node->scroll_y + link->scroll_y;
                if (row < limit)
                    limit = row;
                last = 0;
                link->cursor_y = limit;
                if (link->has_scroll)
                    last = limit == link->y_limit - 1;
                changed = 1;
                g_MenuWidgetCurrentNode = link;
                margin = last + 1;
                link->cursor_x = link->grid_width - margin;
                Menu_PlayMoveSound();
            }
        }
        held = MenuWidget_HeldShoulder();
        if (node->layout_flags & 1)
            changed |= held;
        else
            changed |= 1;
    } else if (buttons & 0x2000) {
        int x = node->cursor_x;

        if (x >= 0) {
            int last;

            last = 0;
            if (node->cursor_y == node->y_limit - 1)
                last = node->has_scroll != 0;
            margin = last + 1;
            if (x < node->x_limit - margin) {
                node->cursor_x = x == 4 ? 6 : x + 1;
                Menu_PlayMoveSound();
                changed = 1;
            } else {
                link = node->linkedNext;
                if (link != 0) {
                    node->cursor_x = -1;
                    link->cursor_x = 0;
                    limit = link->y_limit - 1;
                    row = node->cursor_y - node->scroll_y + link->scroll_y;
                    if (row < limit)
                        limit = row;
                    link->cursor_y = limit;
                    g_MenuWidgetCurrentNode = link;
                    Menu_PlayMoveSound();
                    changed = 1;
                }
            }
        }
        held = MenuWidget_HeldShoulder();
        if (!(node->layout_flags & 2))
            changed |= 1;
        else
            changed |= held;
    } else if (buttons & 0x10000) {
        int from;
        int to;

        if (node->itemAction == 0)
            return changed;
        if (node->target_x >= 0) {
            from = MenuWidget_CursorCell(node);
            to = MenuWidget_TargetCell(node);
            if (from != to) {
                if (node->itemAction(node->selected_base, from, node->selected_base, to)) {
                    node->target_x = -1;
                    Menu_PlayConfirmSound();
                    changed = 1;
                } else {
                    Menu_PlayErrorSound();
                    changed = 1;
                }
            }
        }
        changed |= MenuWidget_SwapMarked(node, node->linkedPrevious);
        changed |= MenuWidget_SwapMarked(node, node->linkedNext);
    } else if (buttons & 0x40) {
        if (node->target_x >= 0) {
            node->cursor_x = node->target_x;
            node->cursor_y = node->target_y;
            node->target_x = -1;
            MenuWidget_ScrollToCursor(node);
            if (node->refreshItems != 0)
                node->refreshItems();
            Menu_PlayCancelSound();
            changed = 1;
        }
        changed |= MenuWidget_RestoreLinked(node, node->linkedPrevious);
        changed |= MenuWidget_RestoreLinked(node, node->linkedNext);
    } else if (node->popup_node != 0 && node == g_MenuWidgetCurrentNode) {
        if (buttons & 4) {
            row = node->cursor_y - node->visible_rows;
            if (row < 0)
                row = 0;
            node->cursor_y = row;
            Menu_StepListNavigate(node->popup_node, 0x1000);
            return changed;
        }
        if (buttons & 8) {
            node->cursor_y =
                node->y_limit - MenuWidget_EndMargin(node) < node->cursor_y + node->visible_rows
                    ? node->y_limit - MenuWidget_EndMargin(node)
                    : node->cursor_y + node->visible_rows;
            Menu_StepListNavigate(node->popup_node, 0x4000);
            return changed;
        }
    } else if (buttons & 0x20) {
        changed = 1;
    }
    return changed;
}
