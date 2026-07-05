int StartPAD2(void);
void ChangeClearPAD(int value);
void EnablePAD(void);

/* CC1_PROFILE: build0001 */
extern int g_InitPadFlag;

extern void DisablePAD(void);
extern void StopPAD2(void);
extern void RemovePatchPad(void);

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
