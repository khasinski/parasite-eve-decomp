/* MASPSX_FLAGS: --stack-return-delay */
extern int g_InitPadFlag;

void Pad_StopHandler(void);
void EnterCriticalSection(void);
void Pad_DequeueHandler(void);
void ExitCriticalSection(void);
void ChangeClearPAD(int arg0);
int Sys_SetInterruptHandler(void);
void InitPAD2(int arg0, int arg1, int arg2, int arg3);

void Pad_InitBios(int arg0, int arg1, int arg2, int arg3) {
    Pad_StopHandler();
    EnterCriticalSection();
    Pad_DequeueHandler();
    ExitCriticalSection();
    ChangeClearPAD(0);
    Sys_SetInterruptHandler();
    InitPAD2(arg0, arg1, arg2, arg3);
    g_InitPadFlag = 1;
}
