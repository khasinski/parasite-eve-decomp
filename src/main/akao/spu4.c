/* CC1_PROFILE: build0001 */
extern int D_8009D240;
extern int D_8009D260;

extern int g_SpuTransferStatus;
extern int g_AkaoTimerEventDesc;
extern char D_8009B81C[0x40];

void Spu_UploadWithPrepare(int arg0, int arg1);
void Spu_WaitTransferDone(void);
int StopRCnt(unsigned int arg0);
int UnDeliverEvent(unsigned int arg0, int arg1);
int DisableEvent(int arg0);
int CloseEvent(int arg0);
void Spu_WriteKeyOff(unsigned int value);
void SpuQuit(void);

void Spu_SetSampleTableBase(int arg0) {
    D_8009D240 = arg0;
    D_8009D260 = arg0 + 0x800;
}

void Spu_Shutdown(void) {
    unsigned int counter;

    if (g_SpuTransferStatus == 1) {
        Spu_UploadWithPrepare(D_8009B81C, 0x40);
        Spu_WaitTransferDone();
    }

    do {
        counter = 0xF2000002;
    } while (StopRCnt(counter) == 0);

    UnDeliverEvent(0xF2000002, 2);

    while (DisableEvent(g_AkaoTimerEventDesc) == 0) {
    }

    while (CloseEvent(g_AkaoTimerEventDesc) == 0) {
    }

    Spu_WriteKeyOff(0xFFFFFF);
    SpuQuit();
}
