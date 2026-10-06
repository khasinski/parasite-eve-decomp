/* GCC_VERSION: 2.8.1 */

int ChangeClearPAD(int arg0);
int EnterCriticalSection(void);
int ReadInitPadFlag(void);
int InitCARD2(int arg0);
int _copy_memcard_patch(void);
int _patch_card(void);
int _patch_card2(void);
int ExitCriticalSection(void);

void InitCARD(int arg0) {
    ChangeClearPAD(0);
    EnterCriticalSection();
    if (ReadInitPadFlag() == 0) {
        arg0 = 0;
    }
    InitCARD2(arg0);
    _copy_memcard_patch();
    _patch_card();
    _patch_card2();
    ExitCriticalSection();
}
