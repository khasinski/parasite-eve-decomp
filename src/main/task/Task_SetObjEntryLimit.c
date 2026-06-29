void Obj_SetEntryLimit(int arg0, int arg1);

int Task_SetObjEntryLimit(int **arg0) {
    Obj_SetEntryLimit(*arg0[0], *arg0[1]);
    return 1;
}
