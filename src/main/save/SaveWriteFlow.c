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


void Save_StartSelectedWriteSlot(void);
void MemCard_SetDelayedCallback(void (*callback)(void));
void Menu_CreateMemCardProgressWidget(int arg0);
void MenuWidget_NavScrollTo(int selected_base);

void Menu_TriggerSaveWrite(int arg0, int arg1) {
    if (arg1 != 0) {
        MenuWidget_NavScrollTo(0x1F);
        Menu_CreateMemCardProgressWidget(0x45);
        MemCard_SetDelayedCallback(Save_StartSelectedWriteSlot);
    }
}
