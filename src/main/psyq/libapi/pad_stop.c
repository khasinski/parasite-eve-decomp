/* CC1_PROFILE: build0001 */
extern int g_InitPadFlag;

extern void DisablePAD(void);
extern void StopPAD2(void);
extern void RemovePatchPad(void);

void StopPAD(void) {
    DisablePAD();
    StopPAD2();
    RemovePatchPad();
    g_InitPadFlag = 0;
}
