int CD_getsector(void);

int func_8007A8CC(void) {
    return CD_getsector() == 0;
}
