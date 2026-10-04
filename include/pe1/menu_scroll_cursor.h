#ifndef PE1_MENU_SCROLL_CURSOR_H
#define PE1_MENU_SCROLL_CURSOR_H

/* Grid list cursor stepping (Menu_StepScrollCursor). */

#include "pe1/menu_draw_list.h"

void Menu_PlayMoveSound(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);
int Menu_StepListNavigate(MenuWidgetNode *list, unsigned int flags);
int Menu_StepScrollCursor(MenuWidgetNode *node, unsigned int buttons);

#endif
