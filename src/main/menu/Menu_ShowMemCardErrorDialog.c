/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_McDialogMode;

int MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);
void Menu_CreateTwoLineDialog(int arg0, int arg1);

void Menu_ShowMemCardErrorDialog(int arg0) {
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateTwoLineDialog(arg0 + 0x47, g_McDialogMode + 0x42);
    }
}
