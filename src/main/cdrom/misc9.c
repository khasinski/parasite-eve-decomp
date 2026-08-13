
void CdRom_AsyncCallback(void);
int DsSyncCallback();

extern int g_DsReadBusy;

int DsRead_IsBusy(void);

void CdRom_SeekDoneCallback(unsigned char arg0) {
    if (arg0 == 2) {
        DsSyncCallback((void (*)(void))0x80081E70);
    }
}

int DsRead_IsBusy(void) {
    return g_DsReadBusy;
}
