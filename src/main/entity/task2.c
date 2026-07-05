extern int *g_CurrentEntity;

void Entity_ResolvePosition(char *actor, int index);

int Menu_GetEquipSlotStateOrIndex(void);

void Task_EnableMovement(void);

void Task_DisableMovement(void);

int Entity_ResolveCurrentPosition(unsigned short **arg0) {
    Entity_ResolvePosition(g_CurrentEntity, *arg0[0]);
    return 1;
}

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
