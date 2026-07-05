/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void (*g_MenuDeferredCallback)(void);

void MenuWidget_DestroyNode(void);
void Menu_PlayConfirmSound(void);

void Menu_SetDeferredCallback(void (*callback)(void)) {
    g_MenuDeferredCallback = callback;
}

int Menu_HandleDeferredCallbackInput(int arg0, int arg1) {
    if ((arg1 & 0x10040) != 0) {
        MenuWidget_DestroyNode();
        if (g_MenuDeferredCallback != 0) {
            g_MenuDeferredCallback();
            g_MenuDeferredCallback = 0;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}
