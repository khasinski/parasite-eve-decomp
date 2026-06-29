
void Menu_DrawNotificationItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawNotificationDialogContent(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawNotificationItem);
}
