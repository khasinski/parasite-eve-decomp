/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;

void Save_StartWriteSlot(int arg0, int arg1);

void Save_StartSelectedWriteSlot(void) {
    Save_StartWriteSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}
