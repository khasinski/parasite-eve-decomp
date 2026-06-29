int MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
int MenuWidget_GridCellIndex(int arg0);
void Inv_GetPackedListItem(int arg0);

void Inv_FetchSelectedInventoryItem(void) {
    int node;

    node = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        }
    }

    Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
}
