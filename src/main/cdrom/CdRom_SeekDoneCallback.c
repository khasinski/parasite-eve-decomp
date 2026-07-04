/* MASPSX_FLAGS: --la-call-delay */

void CdRom_AsyncCallback(void);
int DsSyncCallback();

void CdRom_SeekDoneCallback(unsigned char arg0) {
    if (arg0 == 2) {
        DsSyncCallback(CdRom_AsyncCallback);
    }
}
