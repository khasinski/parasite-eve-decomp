void Obj_SetEntryField8(int arg0, int arg1);

int Task_SetObjEntryField8(int **arg0) {
    Obj_SetEntryField8(*arg0[0], *arg0[1]);
    return 1;
}
