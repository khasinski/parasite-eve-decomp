/* GCC_VERSION: 2.8.1 */

/* CC1_FLAGS: -mno-split-addresses */

extern volatile unsigned char *D_8009B27C;
extern unsigned char D_8009B295;
extern unsigned char D_800A3468[], D_800A3460[];
extern void (*D_8009AFB8)(int, unsigned char *);
extern void (*D_8009AFB4)(int, unsigned char *);
int getintr(void);
void Cd_SetIntrMask(void) {
    int pending;
    unsigned char *ready = &D_8009B295;
    int bank = *D_8009B27C & 3;
    unsigned char *complete = ready - 1;
    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8) D_8009AFB8(*ready, D_800A3468);
        if ((pending & 2) && D_8009AFB4) D_8009AFB4(*complete, D_800A3460);
    }
    *D_8009B27C = bank;
}
