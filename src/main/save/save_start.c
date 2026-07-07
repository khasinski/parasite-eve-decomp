/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;

void Save_StartWriteSlot(int arg0, int arg1);

int Save_StartReadSlot(int port, int slot);

void Save_StartSelectedWriteSlot(void) {
    Save_StartWriteSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}

void Save_StartSelectedReadSlot(void) {
    Save_StartReadSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}
