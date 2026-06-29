void Pm_SetGetState(int arg0, int arg1, int arg2);

int Pm_ScriptSetState(int **arg0) {
    Pm_SetGetState(*arg0[0], 0, *arg0[1]);
    return 1;
}
