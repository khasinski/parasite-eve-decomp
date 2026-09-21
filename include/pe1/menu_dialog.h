#ifndef PE1_MENU_DIALOG_H
#define PE1_MENU_DIALOG_H

#include "pe1/menu_inventory.h"

MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent,
                                           int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent,
                                     MenuWidgetNode *sibling);
void Menu_DrawNotificationText(MenuWidgetNode *node);
void Menu_DrawTwoLineDialogText(void);
void Menu_DrawNotificationDialogContent(void);
int Menu_HandleDeferredCallbackInput(int arg0, int arg1);
void Menu_SetDeferredCallback(void (*callback)(void));

/* Two adjacent 64-byte, 0xFF-terminated notification text buffers. */
extern u8 D_800A1A20[2][64];
extern u8 g_MenuTwoLineDialogText[];

#endif
