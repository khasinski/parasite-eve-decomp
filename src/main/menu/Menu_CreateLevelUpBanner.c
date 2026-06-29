void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void Menu_DrawLevelUpBanner(void);

void Menu_CreateLevelUpBanner(void) {
    *(void **)((char *)MenuWidget_CreateSimpleNode(0x16, 0, 0, 0) + 0x30) = Menu_DrawLevelUpBanner;
}
