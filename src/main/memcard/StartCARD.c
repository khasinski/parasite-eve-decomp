extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearPAD(int);
extern void StartCARD2(void);

long StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}
