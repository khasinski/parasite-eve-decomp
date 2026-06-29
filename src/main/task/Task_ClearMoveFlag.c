void Task_DisableMovement(void);

int Task_ClearMoveFlag(void) {
    Task_DisableMovement();
    return 1;
}
