extern int CD_getsector2(void);

extern int CD_getsector(void);

extern int CD_datasync(void);

int CdRom_DataSync(void);

int CdRom_IsBusy(void) {
    return CD_getsector2() == 0;
}

int CdRom_IsBusy2(void) {
    return CD_getsector() == 0;
}

int CdRom_DataSync(void) {
    return CD_datasync();
}
