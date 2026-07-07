
int Menu_GetEquipSlotIndex(void);

int Task_GetMenuFlag(int **arg0) {
    int value;

    value = Menu_GetEquipSlotIndex();
    **arg0 = (signed char)value;
    return 1;
}
