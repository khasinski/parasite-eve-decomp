
void Menu_DrawParasiteAbilityItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawParasiteAbilityList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawParasiteAbilityItem);
}
