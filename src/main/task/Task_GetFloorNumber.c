int Menu_GetEquipSlotStateOrIndex(void);

int Task_GetFloorNumber(int **arg0) {
    *arg0[0] = (unsigned char)Menu_GetEquipSlotStateOrIndex();
    return 1;
}
