/* CC1_PROFILE: build0001 */
/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

typedef struct WrapperArgs {
    int *arg0;
    int *arg2;
    int *arg3;
    int *arg4;
    int *arg5;
} WrapperArgs;

void Pm_SendCmd(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

int Pm_ScriptSendCommand0(WrapperArgs *args) {
    register int *ptr0 asm("$5");
    register int *ptr2 asm("$3");
    register int *ptr3 asm("$3");
    int *ptr4;
    register int value4 asm("$2");
    register int value0 asm("$5");
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
