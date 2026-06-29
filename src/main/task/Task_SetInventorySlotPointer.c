extern char *g_CurrentEntity;
extern int *D_8009D248;
extern short D_8009D1CC;

int Inv_CountTotal(void);
int Inv_GetAyaSlotLimit(void);

int Task_SetInventorySlotPointer(int **arg0) {
    D_8009D248 = (int *)((char *)*(int **)(g_CurrentEntity + 0x9C) + (*arg0[0] << 1));
    D_8009D1CC = *arg0[1];
    return 1;
}

int Task_GetInventoryTotal(int **arg0) {
    *arg0[0] = Inv_CountTotal();
    return 1;
}

int Task_GetInventorySlotLimit(int **arg0) {
    *arg0[0] = Inv_GetAyaSlotLimit();
    return 1;
}
