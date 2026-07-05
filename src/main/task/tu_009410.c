int Obj_GetEntryField6(int arg0);

void Obj_SetEntryField8(int arg0, int arg1);

void Obj_FillEntrySlotValues(int arg0, int arg1);

void Obj_SetEntrySlotValue(int arg0, int arg1, int arg2);

void Obj_SetEntryFlags(int arg0, int arg1);

int Task_GetObjEntryField6(int **arg0) {
    *arg0[1] = Obj_GetEntryField6(*arg0[0]);
    return 1;
}

int Task_SetObjEntryField8(int **arg0) {
    Obj_SetEntryField8(*arg0[0], *arg0[1]);
    return 1;
}

int Task_FillObjEntrySlotValues(int **arg0) {
    Obj_FillEntrySlotValues(*arg0[0], *arg0[1]);
    return 1;
}

int Task_SetObjEntrySlotValue(int **arg0) {
    Obj_SetEntrySlotValue(*arg0[0], *arg0[1], *arg0[2]);
    return 1;
}

int Task_SetObjEntryFlags(int **arg0) {
    Obj_SetEntryFlags(*arg0[0], *arg0[1]);
    return 1;
}

int Task_ClearObjEntryFlags(int **arg0) {
    Obj_SetEntryFlags(*arg0[0], ~*arg0[1]);
    return 1;
}
