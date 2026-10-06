/* PSY-Q LIBCARD INIT, part 2 of 2: StartCARD, StopCARD. */
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearPAD(int);
extern void StartCARD2(void);

extern void StopCARD2(void);
extern void _ExitCard(void);

long StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}

long StopCARD(void) {
    StopCARD2();
    _ExitCard();
}
