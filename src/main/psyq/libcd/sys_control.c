void func_8007B964(void *arg0);

int func_8007A88C(void *arg0) {
    func_8007B964(arg0);
    return 1;
}
int CD_getsector2(void);

int CdGetSector2(void) {
    return CD_getsector2() == 0;
}
int CD_getsector(void);

int func_8007A8CC(void) {
    return CD_getsector() == 0;
}
