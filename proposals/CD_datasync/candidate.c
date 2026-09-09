/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS:  */
extern int VSync(int);
extern int puts(const char *);
extern int printf(const char *, ...);
extern int CD_flush(void);
extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011C20[], D_80011B18[], D_80011B28[];
extern unsigned char D_8009B294[2], D_8009AFD5;
extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile unsigned int *D_8009B2C0;
static inline int timeout(void) {
    if (VSync(-1) > D_800A3478 || D_800A347C++ > 0x3c0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, D_8009AFDC[D_8009AFD5],
               D_8009B05C[D_8009B294[0]], D_8009B05C[D_8009B294[1]]);
        CD_flush();
        return -1;
    }
    return 0;
}
int CD_datasync(int mode) {
    D_800A3478 = VSync(-1) + 0x3c0;
    D_800A347C = 0;
    D_800A3480 = D_80011C20;
    do {
        if (timeout()) return -1;
        if (!(*D_8009B2C0 & 0x1000000)) return 0;
    } while (!mode);
    return 1;
}
