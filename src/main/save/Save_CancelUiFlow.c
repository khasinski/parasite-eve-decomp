void MenuWidget_NavScrollTo(int arg0);
void Inv_SetActiveList(int arg0, int arg1);

void Save_CancelUiFlow(void) {
    MenuWidget_NavScrollTo(0x26);
    MenuWidget_NavScrollTo(0x25);
    MenuWidget_NavScrollTo(0x24);
    Inv_SetActiveList(0xC, 0);
}
