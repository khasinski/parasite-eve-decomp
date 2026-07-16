
typedef struct WrapperArgs {
    int *arg0;
    int *arg2;
    int *arg3;
    int *arg4;
    int *arg5;
} WrapperArgs;

int Pm_SendCmd(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

int Pm_ScriptSendCommand0(WrapperArgs *args) {
    int *ptr0;
    int *ptr2;
    int *ptr3;
    int *ptr4;
    int value4;
    int value0;
    int value2;

    ptr0 = args->arg0;
    ptr4 = args->arg4;
    ptr2 = args->arg2;
    value4 = *ptr4;
    value0 = *ptr0;
    value2 = *ptr2;
    ptr3 = args->arg3;
    Pm_SendCmd(value0, 0, value2, *(volatile int *)ptr3, value4, *(volatile int *)args->arg5);
    return 1;
}
