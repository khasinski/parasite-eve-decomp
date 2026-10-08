#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

void (*g_MenuDeferredCallback)(void);

void Menu_PlayConfirmSound(void);

void Menu_SetDeferredCallback(void (*callback)(void)) {
    g_MenuDeferredCallback = callback;
}

int Menu_HandleDeferredCallbackInput(MenuWidgetNode *node, int arg1) {
    if ((arg1 & 0x10040) != 0) {
        MenuWidget_DestroyNode(node);
        if (g_MenuDeferredCallback != 0) {
            g_MenuDeferredCallback();
            g_MenuDeferredCallback = 0;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}
