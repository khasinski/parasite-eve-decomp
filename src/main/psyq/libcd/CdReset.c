/* MASPSX_FLAGS: --stack-return-delay */

void CD_initintr(void);
int CD_init(void);
int CD_initvol(void);

int CdReset(int arg0) {
    register int ret asm("$3");

    if (arg0 == 2) {
        CD_initintr();
        return 1;
    }

    if (CD_init() != 0) {
        return 0;
    }

    if (arg0 == 1) {
        ret = CD_initvol();
        asm volatile("" : "=r"(ret) : "0"(ret));
        if (ret != 0) {
            return 0;
        }
    }

    return 1;
}
