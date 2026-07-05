
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
