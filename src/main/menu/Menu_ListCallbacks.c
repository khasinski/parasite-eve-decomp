void Menu_DrawMainMenuItem(int arg0);
void MenuWidget_DrawList();

void Menu_DrawMainMenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawMainMenuItem);
}

void Menu_DrawActionSubmenuItem(int arg0);

void Menu_DrawActionSubmenuList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawActionSubmenuItem);
}

void Menu_DrawBlendOptionItem(int arg0);

void Menu_DrawBlendColorOptionListUnk(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawBlendOptionItem);
}

void Menu_DrawNotificationItem(void);

void Menu_DrawNotificationDialogContent(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawNotificationItem);
}

void Menu_DrawInvPanelItemStub(void);

void Menu_DrawItemListInvPanel(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawInvPanelItemStub);
}

void Menu_DrawStatAllocationItem(int arg0);
extern int g_StatBaseTable[];

void Menu_DrawStatAllocationList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawStatAllocationItem);
}

int Menu_GetStatBaseValue(int index) {
    return g_StatBaseTable[index];
}

void Menu_DrawScreenModeItem(int arg0);

void Menu_DrawScreenModeList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawScreenModeItem);
}

void Menu_DrawParasiteAbilityItem(int arg0);

void Menu_DrawParasiteAbilityList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawParasiteAbilityItem);
}

void Menu_DrawEmptyList(int arg0) {
    MenuWidget_DrawList(arg0, 0);
}
