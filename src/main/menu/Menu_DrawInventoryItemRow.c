int Inv_IsAyaInventorySlotReserved(void *arg0);
int Inv_TestSelectionBit(void *arg0);
void *Inv_GetWayneListItemByIndex(void);
void Sfx_DrawActiveListSlot(void *arg0);
void Draw_SetTextDimmed(int arg0);
void Draw_DropShadow(void);

void Menu_DrawInventoryItemRow(void) {
    void *obj = Inv_GetWayneListItemByIndex();

    Draw_SetTextDimmed(Inv_TestSelectionBit(obj) == 0);
    Sfx_DrawActiveListSlot(obj);
    if (Inv_IsAyaInventorySlotReserved(obj) != 0) {
        Draw_DropShadow();
    }
}
