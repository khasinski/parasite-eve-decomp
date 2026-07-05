void Menu_DrawArmorEquipItem(void);
void Inv_SelectActiveList(int useOverride);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

int MemCard_IsPortPresent(int arg0);

void Menu_DrawArmorSelectionList(int arg0) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(arg0, Menu_DrawArmorEquipItem);
}

int Menu_IsMemCardSlotSelectable(int arg0) {
    int ret;

    ret = 0;
    if (arg0 == 2) {
        ret = 1;
    } else if (MemCard_IsPortPresent(arg0) != 0) {
        ret = 1;
    }
    return ret;
}
