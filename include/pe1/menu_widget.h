#ifndef PE1_MENU_WIDGET_H
#define PE1_MENU_WIDGET_H

#include "common.h"

typedef struct MenuWidgetNode {
    struct MenuWidgetNode *next;
    struct MenuWidgetNode *parent;
    struct MenuWidgetNode *children[4];
    int x;
    int y;
    int mode;
    int selected_base;
    int field_28;
    void (*update)();          /* +0x2C callback/handler (generic: holds void(void) or int(int,int) etc.) */
    void (*field_30)();         /* +0x30 draw/update callback */
    int grid_width;
    int visible_rows;
    int draw_state;
    int disabled;
    int cursor_x;
    int cursor_y;
    int target_x;
    int target_y;
    int x_limit;
    int y_limit;
    int scroll_y;
    int scroll_adjust;
    int layout_flags;
    int has_scroll;
    int visible_rows_mirror;
    int aux_index;            /* initialized from mode; reused by some widget kinds */
    int cell_mask;
    struct MenuWidgetNode *field_78;
    struct MenuWidgetNode *field_7C;
    struct MenuWidgetNode *popup_node;
    int field_84;
    int field_88;
    int field_8C;
} MenuWidgetNode;

PE1_STATIC_ASSERT(sizeof(MenuWidgetNode) == 0x90, menu_widget_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, parent) == 0x04,
                  menu_widget_parent_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, children) == 0x08,
                  menu_widget_children_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, x) == 0x18,
                  menu_widget_x_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, update) == 0x2C,
                  menu_widget_update_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, cursor_x) == 0x44,
                  menu_widget_cursor_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, scroll_y) == 0x5C,
                  menu_widget_scroll_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, aux_index) == 0x70,
                  menu_widget_aux_index_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetNode, popup_node) == 0x80,
                  menu_widget_popup_node_offset);

typedef struct MenuWidgetSimpleDescriptor {
/* 0x00 */ s32 x;      /* zero requests horizontal centering */
/* 0x04 */ s32 y;      /* when x is zero, nonzero selects the upper anchor */
/* 0x08 */ s32 width;
/* 0x0C */ s32 height;
} MenuWidgetSimpleDescriptor;

typedef struct MenuWidgetGridDescriptor {
/* 0x00 */ s32 x;
/* 0x04 */ s32 y;
/* 0x08 */ s32 gridWidth;
/* 0x0C */ s32 visibleRows;
/* 0x10 */ s32 yLimit;
/* 0x14 */ s32 initialDrawState;
/* 0x18 */ s32 initialDisabled;
/* 0x1C */ s32 layoutFlags;
} MenuWidgetGridDescriptor;

PE1_STATIC_ASSERT(sizeof(MenuWidgetSimpleDescriptor) == 0x10,
                  menu_widget_simple_descriptor_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetSimpleDescriptor, width) == 0x08,
                  menu_widget_simple_descriptor_width_offset);
PE1_STATIC_ASSERT(sizeof(MenuWidgetGridDescriptor) == 0x20,
                  menu_widget_grid_descriptor_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuWidgetGridDescriptor, yLimit) == 0x10,
                  menu_widget_grid_descriptor_y_limit_offset);

MenuWidgetSimpleDescriptor *MenuWidget_LookupSimpleDescriptor(unsigned int index);
MenuWidgetGridDescriptor *MenuWidget_LookupGridDescriptor(unsigned int index);
void Draw_SetPrimCallback(MenuWidgetNode *node, int item_count);
void MenuWidget_DestroyPopupNode(MenuWidgetNode *node);

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
 *   and scroll_adjust (+0x60) drive long-list tail/header positioning.
 *   All inventory nodes share update fn 0x80063e0c (the mode-2 step handler);
 *   Menu_InitCategoryTabs builds them via MenuWidget_FindByModeAndSelectedBase(2,..)
 *   + MenuWidget_SetCurrentNode. Tab lookup = find node with mode==2 and the
 *   wanted selected_base.
 * Quick live probe: active tab node = *(u32*)0x8009D15C; tab id =
 *   node->selected_base (+0x24); cursor = node->cursor_x/cursor_y (+0x44/+0x48).
 */

#endif
