/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

extern int g_MenuEquipMode;

void Menu_OpenEquipScreen(int arg0);

void Menu_ReopenEquipScreen(void) {
    Menu_OpenEquipScreen(g_MenuEquipMode - 1);
}
