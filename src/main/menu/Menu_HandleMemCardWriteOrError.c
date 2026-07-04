/* MASPSX_FLAGS: -G8 */
/* CC1_FLAGS: -G8 */

extern int g_MemCardProgressPrimList;
extern void (*g_MenuDeferredCallback)(void);

void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void MemCard_MarkActivePortState13(void);
void MemCard_ClearActivePrompt(void);
int MemCard_GetActivePort(void);
void MemCard_SetDelayedCallback(void (*callback)(void));
void Menu_CreateTwoLineDialog(int arg0, int arg1);
void MenuWidget_SetCurrentNode(char *node);
int MenuWidget_GetCurrentNode(void);
char *MenuWidget_CreateSimpleNode(int arg0, int arg1, int arg2, int arg3);

void Menu_HandleMemCardWriteOrError(int arg0, int arg1) {
    char *entry;

    if (arg1 != 0) {
        entry = MenuWidget_CreateSimpleNode(0x27, MenuWidget_GetCurrentNode(), 0, 1);
        *(void (**)(void))(entry + 0x30) = MemCard_DrawProgress;
        *(int (**)(void))(entry + 0x2C) = Menu_MemCardProgressInputHandler;
        MenuWidget_SetCurrentNode(entry);
        g_MemCardProgressPrimList = 0x40;
        MemCard_SetDelayedCallback(MemCard_MarkActivePortState13);
    } else {
        Menu_CreateTwoLineDialog(MemCard_GetActivePort() + 0x47, 0x4B);
        g_MenuDeferredCallback = MemCard_ClearActivePrompt;
    }
}
