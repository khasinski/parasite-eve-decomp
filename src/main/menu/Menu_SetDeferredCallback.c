/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void (*g_MenuDeferredCallback)(void);

void Menu_SetDeferredCallback(void (*callback)(void)) {
    g_MenuDeferredCallback = callback;
}
