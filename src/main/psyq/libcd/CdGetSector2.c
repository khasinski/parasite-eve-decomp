extern int CD_getsector2(void);

int CdGetSector2(void) {
    return CD_getsector2() == 0;
}
