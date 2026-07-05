void Menu_DrawStatAllocationItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

extern int g_StatBaseTable[];

void Menu_DrawStatAllocationList(int arg0) {
    MenuWidget_DrawList(arg0, Menu_DrawStatAllocationItem);
}

int Menu_GetStatBaseValue(int index) {
    return g_StatBaseTable[index];
}
