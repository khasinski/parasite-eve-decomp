void Task_EnableMovement(void);

int Task_SetMoveFlag(void) {
    Task_EnableMovement();
    return 1;
}
