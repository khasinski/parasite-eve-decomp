extern int *g_CurrentEntity;

void Pm_Stop(int arg0, int *arg1, int arg2);

int Pm_ScriptStopCurrentEntity(int **arg0) {
    Pm_Stop(*arg0[0], g_CurrentEntity, 0);
    return 1;
}
