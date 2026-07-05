/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern unsigned int g_AkaoPendingBgmVolumeFade;

extern void Akao_Cmd_21(unsigned int arg0, int arg1);

extern unsigned int g_MenuActionSubmenuSel;

extern void Akao_Cmd_80_81_82(int arg0);

void Akao_FlushBgmVolumeFade(void) {
    if (g_AkaoPendingBgmVolumeFade != 0) {
        Akao_Cmd_21(g_AkaoPendingBgmVolumeFade, 0);
        g_AkaoPendingBgmVolumeFade = 0;
    }
}

void Menu_SetActionSubmenuSelection(int value) {
    g_MenuActionSubmenuSel = value;
    Akao_Cmd_80_81_82(value == 0);
}

unsigned int Menu_GetActionSubmenuSelection(void) {
    return g_MenuActionSubmenuSel;
}
