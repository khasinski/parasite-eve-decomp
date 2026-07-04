extern int CD_getsector2(void);

int CdRom_IsBusy(void) {
    return CD_getsector2() == 0;
}
