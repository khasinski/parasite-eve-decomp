/* CC1_FLAGS: -g3 */

int MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);

int Menu_IsMemCardDialogOpen(void) {
    return MenuWidget_FindByModeAndSelectedBase(1, 0x24) != 0;
}
