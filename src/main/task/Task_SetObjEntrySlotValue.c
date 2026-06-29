void Obj_SetEntrySlotValue(int arg0, int arg1, int arg2);

int Task_SetObjEntrySlotValue(int **arg0) {
    Obj_SetEntrySlotValue(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}
