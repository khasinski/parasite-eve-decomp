/* CC1_VERSION: 2.8.1 */
void CD_initintr(void);
int CD_init(void);
int CD_initvol(void);

int CdReset(int arg0) {
    if (arg0 == 2) {
        CD_initintr();
        return 1;
    }

    if (CD_init() != 0) {
        return 0;
    }

    if (arg0 == 1) {
        if (CD_initvol() != 0) {
            return 0;
        }
    }

    return 1;
}
