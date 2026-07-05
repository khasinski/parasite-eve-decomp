int StartPAD2(void);
void ChangeClearPAD(int value);
void EnablePAD(void);

extern int g_InitPadFlag;

extern void DisablePAD(void);
extern void StopPAD2(void);
int RemovePatchPad(void);

int Pad_Start(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
    return 1;
}

void StopPAD(void) {
    DisablePAD();
    StopPAD2();
    RemovePatchPad();
    g_InitPadFlag = 0;
}
