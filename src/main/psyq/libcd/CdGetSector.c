extern int CD_getsector(void);

int CdGetSector(void) {
    return CD_getsector() == 0;
}
