/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuActionTextBase;
int g_MenuSelectedItemIndex;

void *MenuWidget_GetCurrentNode(void);
void *MenuWidget_CreateSimpleNode(int arg0, void *arg1, int arg2, int arg3);
void *MenuWidget_CreateNode(int arg0, void *arg1, void *arg2);
void MenuWidget_SetCurrentNode(void *arg0);
void Menu_DrawItemActionSubmenu(void);
void Menu_StepSkillSelect(void);
void Menu_DrawActionOptionList(void) __asm__("func_8004F950");

void Menu_CreateItemActionSubmenu(void) {
    void *node;
    void *child;

    node = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(4, node, node);
    *(void **)((char *)node + 0x30) = Menu_DrawItemActionSubmenu;
    *(void **)((char *)node + 0x2C) = Menu_StepSkillSelect;
    *(void **)((char *)child + 0x30) = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuSelectedItemIndex & 1) {
        *(int *)((char *)node + 0x38) -= 0x1C;
        *(int *)((char *)child + 0x1C) -= 0x1C;
    }
}
