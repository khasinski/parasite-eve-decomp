/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int MenuWidget_FindByModeAndSelectedBase(int arg0, int arg1);

int Menu_IsMemCardDialogOpen(void) {
    return MenuWidget_FindByModeAndSelectedBase(1, 0x24) != 0;
}
