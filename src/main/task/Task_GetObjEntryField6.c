int Obj_GetEntryField6(int arg0);

int Task_GetObjEntryField6(int **arg0) {
    *arg0[1] = Obj_GetEntryField6(*arg0[0]);
    return 1;
}
