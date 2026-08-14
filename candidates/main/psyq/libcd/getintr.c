/* CC1_FLAGS: -fno-strength-reduce */

extern int puts(const char *s);
extern int printf(const char *fmt, ...);

extern volatile unsigned char *D_8009B27C;
extern volatile unsigned char *D_8009B280;
extern volatile unsigned char *D_8009B284;
extern volatile unsigned char *D_8009B288;

extern unsigned char D_8009B294[];
extern unsigned char D_8009AFD5;
extern int D_8009AFC4;
extern int D_8009AFC8;
extern int D_8009AFCC;
extern int D_8009AFC0;
extern int D_8009B17C[];
extern int D_8009B07C[];
extern char *D_8009AFDC[];

extern unsigned char D_800A3460[];
extern unsigned char D_800A3468[];
extern unsigned char D_800A3470[];

extern char D_80011B44[];
extern char D_80011B50[];
extern char D_80011B6C[];
extern char D_80011B80[];

static inline void _memcpy(void *_dst, void *_src, unsigned int _size) {
    char *pDst = (char *)_dst;
    char *pSrc = (char *)_src;

    if (pDst == 0) {
        return;
    }

    while (_size--) {
        *pDst++ = *pSrc++;
    }
}

int getintr(void) {
    volatile unsigned char nReg;
    volatile unsigned char buf[8];
    int i, j;
    int bHasError;

    *D_8009B27C = 1;

    nReg = *D_8009B288 & 0x7;

    if (nReg == 0) {
        return 0;
    }

    bHasError = 0;

    while (nReg != (*D_8009B288 & 7)) {
        nReg = *D_8009B288 & 0x7;
    }

    for (i = 0; i < 8; i++) {
        if ((*D_8009B27C & 0x20) == 0) {
            break;
        }
        buf[i] = *D_8009B280;
    }
    for (j = i; j < 8; j++) {
        buf[j] = 0;
    }

    *D_8009B27C = 1;
    *D_8009B288 = 7;
    *D_8009B284 = 7;

    if (nReg != 3 || D_8009B17C[D_8009AFD5]) {
        if (!(D_8009AFC4 & 0x10) && (buf[0] & 0x10)) {
            D_8009AFCC++;
        }
        D_8009AFC4 = buf[0];
        D_8009AFC8 = buf[1];
        bHasError = D_8009AFC4;
        bHasError &= 0x1D;
    }

    if (nReg == 5) {
        if (D_8009AFC0 > 0) {
            printf(D_80011B44);
        }
        if (D_8009AFC0 > 0) {
            printf(D_80011B50, D_8009AFDC[D_8009AFD5], D_8009AFC4, D_8009AFC8);
        }
    }

    switch (nReg) {
    case 3:
        if (bHasError) {
            D_8009B294[0] = 5;
            _memcpy(D_800A3460, (void *)buf, 8);
            return 2;
        }
        if (D_8009B07C[D_8009AFD5]) {
            D_8009B294[0] = 3;
            _memcpy(D_800A3460, (void *)buf, 8);
            return 1;
        }
        D_8009B294[0] = 2;
        _memcpy(D_800A3460, (void *)buf, 8);
        return 2;
    case 2:
        D_8009B294[0] = bHasError ? 5 : 2;
        _memcpy(D_800A3460, (void *)buf, 8);
        return 2;
    case 1:
        if (bHasError && i == 1) {
            bHasError = 0;
        }
        D_8009B294[1] = bHasError ? 5 : 1;
        _memcpy(D_800A3468, (void *)buf, 8);
        *D_8009B27C = 0;
        *D_8009B288 = 0;
        return 4;
    case 4:
        D_8009B294[2] = 4;
        D_8009B294[1] = D_8009B294[2];
        _memcpy(D_800A3470, (void *)buf, 8);
        _memcpy(D_800A3468, (void *)buf, 8);
        return 4;
    case 5:
        D_8009B294[1] = 5;
        D_8009B294[0] = D_8009B294[1];
        _memcpy(D_800A3460, (void *)buf, 8);
        _memcpy(D_800A3468, (void *)buf, 8);
        return 6;
    default:
        puts(D_80011B6C);
        printf(D_80011B80, nReg);
        return 0;
    }
}
