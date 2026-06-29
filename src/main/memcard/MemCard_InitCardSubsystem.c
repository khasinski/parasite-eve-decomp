/* MASPSX_FLAGS: --stack-return-delay */

int ChangeClearPAD(int arg0);
int EnterCriticalSection(void);
int ReadInitPadFlag(void);
int InitCARD2(int arg0);
int func_8007E4E0(void);
int func_8007E3DC(void);
int func_8007E470(void);
int ExitCriticalSection(void);

void MemCard_InitCardSubsystem(int arg0) {
    ChangeClearPAD(0);
    EnterCriticalSection();
    if (ReadInitPadFlag() == 0) {
        arg0 = 0;
    }
    InitCARD2(arg0);
    func_8007E4E0();
    func_8007E3DC();
    func_8007E470();
    ExitCriticalSection();
}
