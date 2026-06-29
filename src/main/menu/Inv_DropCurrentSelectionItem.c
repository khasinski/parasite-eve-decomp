void *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base);
int MenuWidget_GridCellIndex(void *node);
void Inv_RemoveActiveListItem(int arg0);

void Inv_DropCurrentSelectionItem(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(2, 1);
    int index;

    if (node != 0) {
        index = MenuWidget_GridCellIndex(node);
        if (index >= 0) {
            Inv_RemoveActiveListItem(index);
        }
    }
}
