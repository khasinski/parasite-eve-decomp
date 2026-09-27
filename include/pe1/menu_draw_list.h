#ifndef PE1_MENU_DRAW_LIST_H
#define PE1_MENU_DRAW_LIST_H

#include "pe1/menu_widget.h"

extern int D_8009D0E8;
extern int D_8009D164, D_8009D168;

void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw_callback)(int));
void MenuWidget_DrawListRow(MenuWidgetNode *node, void (*draw_callback)(int),
                            int row, int draw_cursor);
void MenuWidget_EaseNodePosition(MenuWidgetNode *node);
int Draw_RemapStatusFlags(void);
void Draw_AllocColorTriGradient(int width, int height, int mode, int focused);

#endif
