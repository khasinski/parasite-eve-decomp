extern int g_SpuTransferStatus;
extern int g_AkaoTimerEventDesc;
extern char D_8009B81C[0x40];

void Spu_UploadWithPrepare(void *arg0, int arg1);
void Spu_WaitTransferDone(void);
int StopRCnt(unsigned int arg0);
int UnDeliverEvent(unsigned int arg0, int arg1);
int DisableEvent(int arg0);
int CloseEvent(int arg0);
void Spu_WriteKeyOff(unsigned int arg0);
void SpuQuit(void);

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
