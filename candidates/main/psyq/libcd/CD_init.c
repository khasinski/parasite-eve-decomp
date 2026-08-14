/* CC1_FLAGS: -fno-schedule-insns */

extern int puts(const char *s);
extern int printf(const char *fmt, ...);
extern void ResetCallback(void);
extern void InterruptCallback(int event, void (*callback)(void));
extern void Cd_SetIntrMask(void);
extern int CD_cw(int com, void *param, void *result, int arg3);
extern int CD_sync(int mode, void *result);

extern char D_80011C08[];
extern char D_80011C14[];
extern int D_8009B298[];

extern unsigned char D_8009AFD4;
extern unsigned char D_8009AFD5;
extern int D_8009AFB4;
extern int D_8009AFB8;
extern int D_8009AFC4;
extern int D_8009AFC8;

extern volatile unsigned char *D_8009B27C;
extern volatile unsigned char *D_8009B284;
extern volatile unsigned char *D_8009B288;
extern volatile unsigned int *D_8009B28C;
extern unsigned char D_8009B294[];

int CD_init(void) {
    puts(D_80011C08);
    printf(D_80011C14, D_8009B298);
    D_8009AFD5 = 0;
    D_8009AFD4 = 0;
    D_8009AFB8 = 0;
    D_8009AFB4 = 0;
    D_8009AFC8 = 0;
    D_8009AFC4 = 0;
    ResetCallback();
    InterruptCallback(2, Cd_SetIntrMask);

    *D_8009B27C = 1;
    while (*D_8009B288 & 7) {
        *D_8009B27C = 1;
        *D_8009B288 = 7;
        *D_8009B284 = 7;
    }

    D_8009B294[2] = 0;
    D_8009B294[1] = D_8009B294[2];
    D_8009B294[0] = 2;

    *D_8009B27C = 0;
    *D_8009B288 = 0;
    *D_8009B28C = 0x1325;

    CD_cw(1, 0, 0, 0);
    if (D_8009AFC4 & 0x10) {
        CD_cw(1, 0, 0, 0);
    }

    if (CD_cw(10, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_cw(12, 0, 0, 0) != 0) {
        return -1;
    }

    if (CD_sync(0, 0) != 2) {
        return -1;
    }

    return 0;
}
