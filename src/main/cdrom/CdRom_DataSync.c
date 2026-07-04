extern int CD_datasync(void);

int CdRom_DataSync(void);

int CdRom_DataSync(void) {
    return CD_datasync();
}
