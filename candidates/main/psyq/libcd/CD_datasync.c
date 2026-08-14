extern int puts(const char *s);
extern int printf(const char *fmt, ...);
extern int VSync(int mode);
extern void CD_flush(void);

extern char D_80011B18[];
extern char D_80011B28[];
extern char D_80011C20[];

extern int D_800A3478;
extern int D_800A347C;
extern char *D_800A3480;

extern char *D_8009AFDC[];
extern char *D_8009B05C[];
extern unsigned char D_8009B294[];
extern unsigned char D_8009AFD5;
extern volatile unsigned int *D_8009B2C0;

int CD_datasync(int mode) {
    char **intrNames;
    unsigned char *intr;
    char **comNames;
    int limit;

    D_800A3478 = VSync(-1) + 960;
    D_800A347C = 0;
    D_800A3480 = D_80011C20;

    limit = 0x3C0000;
    comNames = D_8009AFDC;
    intr = D_8009B294;
    intrNames = D_8009B05C;

    for (;;) {
        if (D_800A3478 < VSync(-1) || D_800A347C++ > limit) {
            puts(D_80011B18);
            printf(D_80011B28, D_800A3480, comNames[D_8009AFD5],
                   intrNames[intr[0]], intrNames[intr[1]]);
            CD_flush();
            return -1;
        }

        if (!(*D_8009B2C0 & 0x01000000)) {
            return 0;
        }

        if (mode != 0) {
            return 1;
        }
    }
}
