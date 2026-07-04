extern int CD_getsector(void);

int CdRom_IsBusy2(void) {
    return CD_getsector() == 0;
}
