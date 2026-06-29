void Obj_SetEntryFlags(int arg0, int arg1);

int Task_SetObjEntryFlags(int **arg0) {
    Obj_SetEntryFlags(*arg0[0], *arg0[1]);
    return 1;
}
