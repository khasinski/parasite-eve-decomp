/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
extern int D_8009CEFC;

int Inv_TestSelectionBit(int slot);
void Draw_SetTextDimmed(int dimmed);
void Sfx_DrawActiveListSlot(int slot);
int Inv_IsAyaInventorySlotReserved(int slot);
void Draw_DropShadow(void);

void Menu_DrawSelectableEquipSlotRow(int slot) {
    int dimmed = 0;

    if (D_8009CEFC != 0 || Inv_TestSelectionBit(slot) == 0) {
        dimmed = 1;
    }

    Draw_SetTextDimmed(dimmed);
    Sfx_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}
