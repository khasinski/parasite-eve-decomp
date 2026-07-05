int Menu_GetEquipSlotStateOrIndex(void);

void Task_EnableMovement(void);

void Task_DisableMovement(void);

int Task_GetFloorNumber(int **arg0) {
    *arg0[0] = (unsigned char)Menu_GetEquipSlotStateOrIndex();
    return 1;
}

int Task_SetMoveFlag(void) {
    Task_EnableMovement();
    return 1;
}

int Task_ClearMoveFlag(void) {
    Task_DisableMovement();
    return 1;
}
