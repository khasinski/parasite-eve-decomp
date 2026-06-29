
void Menu_DrawStatAllocationItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawStatAllocationList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawStatAllocationItem);
}
