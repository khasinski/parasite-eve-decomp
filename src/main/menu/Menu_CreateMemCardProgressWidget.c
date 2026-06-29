/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MemCardProgressPrimList;

void *MenuWidget_GetCurrentNode(void);
void *MenuWidget_CreateSimpleNode(int arg0, void *arg1, int arg2, int arg3);
void MenuWidget_SetCurrentNode(void *arg0);
void MemCard_DrawProgress(void);
void Menu_MemCardProgressInputHandler(void);

void Menu_CreateMemCardProgressWidget(int arg0) {
    void *node;
    void *widget;

    node = MenuWidget_GetCurrentNode();
    widget = MenuWidget_CreateSimpleNode(0x27, node, 0, 1);
    *(void **)((char *)widget + 0x30) = MemCard_DrawProgress;
    *(void **)((char *)widget + 0x2C) = Menu_MemCardProgressInputHandler;
    MenuWidget_SetCurrentNode(widget);
    g_MemCardProgressPrimList = arg0;
}
