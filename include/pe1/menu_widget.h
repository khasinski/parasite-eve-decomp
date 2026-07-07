#ifndef PE1_MENU_WIDGET_H
#define PE1_MENU_WIDGET_H

typedef struct MenuWidgetNode {
    struct MenuWidgetNode *next;
    void *field_04;
    int field_08;
    int field_0C;
    int field_10;
    int field_14;
    int value_18;
    int value_1C;
    int mode;
    int selected_base;
    int field_28;
    void (*update)();          /* +0x2C callback/handler (generic: holds void(void) or int(int,int) etc.) */
    void (*field_30)();         /* +0x30 draw/update callback */
    int grid_width;
    int visible_rows;
    int field_3C;
    int field_40;
    int cursor_x;
    int cursor_y;
    int target_x;
    int target_y;
    int x_limit;
    int y_limit;
    int scroll_y;
    int field_60;
    int field_64;
    int field_68;
    int field_6C;
    int selected_base_mirror;
    int cell_mask;
    struct MenuWidgetNode *field_78;
    struct MenuWidgetNode *field_7C;
    int field_80;
    int field_84;
    int field_88;
    int field_8C;
} MenuWidgetNode;

extern MenuWidgetNode g_MenuWidgetNodePool[];
extern MenuWidgetNode g_MenuWidgetNodePoolSentinel;

/* Active linked-list head walked by the MenuWidget_Find* helpers. */
extern MenuWidgetNode *g_MenuWidgetActiveListHead;
/* Free-list head initialized from D_800A22E0. */
extern MenuWidgetNode *g_MenuWidgetFreeListHead;
/* Current/selected widget node. */
extern MenuWidgetNode *g_MenuWidgetCurrentNode;
/* Saved current node used by SaveAndSet/RestoreCurrentNode. */
extern MenuWidgetNode *g_MenuWidgetSavedNode;

/* MENU TAB / LIST NAVIGATION (verified live via DuckStation GDB while moving the
 * cursor and switching tabs):
 *   D_8009D15C  = the currently FOCUSED node == the active tab/list.
 *                 Switching tabs moves this pointer to a different node; moving
 *                 inside a list changes that node's cursor.
 *   Each tab/category is its own MenuWidgetNode with mode==2 and a distinct
 *   `selected_base` id (observed: 0,1,5,32,58). selected_base identifies the tab
 *   (e.g. node 0x800a2370 sel_base=0 = the scrollable content list; the
 *   0x800a2400/2490/2640/26d0 nodes are the category tabs).
 *   cursor_x / cursor_y (+0x44/+0x48) = position within the focused node
 *   (e.g. (0,N) = row N of the list); y_limit (+0x58), scroll_y (+0x5C),
 *   and field_60 (+0x60) drive long-list tail/header positioning.
 *   All inventory nodes share update fn 0x80063e0c (the mode-2 step handler);
 *   Menu_InitCategoryTabs builds them via MenuWidget_FindByModeAndSelectedBase(2,..)
 *   + MenuWidget_SetCurrentNode. Tab lookup = find node with mode==2 and the
 *   wanted selected_base.
 * Quick live probe: active tab node = *(u32*)0x8009D15C; tab id =
 *   node->selected_base (+0x24); cursor = node->cursor_x/cursor_y (+0x44/+0x48).
 */

#endif
