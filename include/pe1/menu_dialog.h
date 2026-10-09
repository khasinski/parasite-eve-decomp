#ifndef PE1_MENU_DIALOG_H
#define PE1_MENU_DIALOG_H

#include "pe1/menu_inventory.h"
#include "pe1/menu_confirm_callback.h"

void Menu_DrawNotificationText(MenuWidgetNode *node);
void Menu_DrawTwoLineDialogText(void);
void Menu_DrawNotificationDialogContent(void);
int Menu_HandleDeferredCallbackInput(MenuWidgetNode *node, int flags);
void Menu_SetDeferredCallback(void (*callback)(void));

/* Two adjacent 64-byte, 0xFF-terminated notification text buffers. */
extern u8 D_800A1A20[2][64];
extern u8 g_MenuTwoLineDialogText[];

/* Item-action dialogs use two label buffers and their optional primitive lists. */
extern u8 g_MenuItemLabelBuffer[128];
extern int g_MenuItemPrimListHandles[2];
extern int g_MenuActionTextBase;
extern int g_MenuActiveItemList, g_MenuActiveItemSlot;
void Inv_SelectActiveList(int list);
void *Inv_LookupActiveListDisplayData(int index);
void Menu_DrawItemLabel(MenuWidgetNode *node);
int Menu_ConfirmDialogHandler(void *node, int flags);
void Menu_DrawActionOptionList(void *node);
void Menu_OnItemDiscardConfirm(MenuWidgetNode *node, int confirmed);
void Menu_StepItemDetailPanel(void);

#endif
