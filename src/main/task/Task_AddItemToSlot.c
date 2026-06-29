int Inv_CheckSlotUsable(int arg0);

int Task_AddItemToSlot(int **arg0) {
    *arg0[1] = Inv_CheckSlotUsable(*arg0[0]);
    return 1;
}
