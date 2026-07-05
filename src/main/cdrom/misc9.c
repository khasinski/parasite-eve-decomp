/* MASPSX_FLAGS: --la-call-delay */

void CdRom_AsyncCallback(void);
int DsSyncCallback();

extern int g_DsReadBusy;

int DsRead_IsBusy(void);

void CdRom_SeekDoneCallback(unsigned char arg0) {
    if (arg0 == 2) {
        DsSyncCallback(CdRom_AsyncCallback);
    }
}

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
