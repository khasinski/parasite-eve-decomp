/* MASPSX_FLAGS: --stack-return-delay --store-return-delay */

extern int g_InitPadFlag;

void Pad_StopHandler(void);
void EnterCriticalSection(void);
void Pad_DequeueHandler(void);
void ExitCriticalSection(void);
void ChangeClearPAD(int arg0);
int Sys_SetInterruptHandler(void);
void PAD_init2(int arg0, int arg1, int arg2, int arg3);

void SetInitPadFlag(int value) {
    g_InitPadFlag = value;
}

int ReadInitPadFlag(void) {
    return g_InitPadFlag;
}

void Pad_Reinitialize(int arg0, int arg1, int arg2, int arg3) {
    Pad_StopHandler();
    EnterCriticalSection();
    Pad_DequeueHandler();
    ExitCriticalSection();
    ChangeClearPAD(0);
    Sys_SetInterruptHandler();
    PAD_init2(arg0, arg1, arg2, arg3);
    g_InitPadFlag = 1;
}
