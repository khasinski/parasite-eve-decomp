extern char *g_CurrentEntity;
extern short *g_EntityCollisionWallSlot;
extern short g_EntityCollisionWallParam;

extern int g_FieldMoveLock;

int Task_SetAnimSlotPointer(int **arg0) {
    int index = *arg0[0];

    g_EntityCollisionWallSlot = (short *)(*(char **)(g_CurrentEntity + 0x9C) + index * 2);
    g_EntityCollisionWallParam = *arg0[1];
    return 1;
}

int Task_SetInputFlagBit2(void) {
    g_FieldMoveLock |= 4;
    return 1;
}

int Task_ClearInputFlagBit2(void) {
    g_FieldMoveLock &= -5;
    return 1;
}
