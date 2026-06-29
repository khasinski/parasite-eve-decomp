int Pm_SendCmd(int arg0, int arg1, int arg2, int *arg3, int *arg4, int *arg5) __asm__("func_8006F6D4");

int Pm_ScriptSendCommand1(int **arg0) {
    int **base;
    register int *op0 asm("$2");
    register int *op1 asm("$5");
    int *stack0;
    register int first asm("$4");
    int second;

    base = arg0;
    op0 = base[0];
    op1 = base[1];
    stack0 = base[3];
    first = *op0;
    second = *op1;

    Pm_SendCmd(first, 1, second, base[2], stack0, base[4]);
    return 1;
}
