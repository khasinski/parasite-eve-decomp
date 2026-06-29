/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int D_8009CFE4;

void *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);
void MenuWidget_SetCurrentNode(void *arg0);
void Menu_DrawScreenAdjustPanel(void);
void Menu_StepScrollList(void);
int Draw_GetBaseY(void);

void Menu_OpenScreenAdjustView(int arg0) {
    void *node;

    node = MenuWidget_CreateSimpleNode(0x38, arg0, 0, 1);
    *(void **)((char *)node + 0x30) = Menu_DrawScreenAdjustPanel;
    *(void **)((char *)node + 0x2C) = Menu_StepScrollList;
    MenuWidget_SetCurrentNode(node);
    D_8009CFE4 = Draw_GetBaseY();
}
