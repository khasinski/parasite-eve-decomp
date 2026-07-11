/* MASPSX_FLAGS: --la-call-delay --stack-return-delay */

typedef unsigned char u8;

extern volatile u8 *D_8009B27C;
extern u8 D_8009B295;
extern u8 D_800A3460[];
extern u8 D_800A3468[];
extern void (*D_8009AFB4)(u8, u8 *);
extern void (*D_8009AFB8)(u8, u8 *);

int getintr(void);

void Cd_SetIntrMask(void) {
    u8 savedMask;
    u8 *readyStatus;
    u8 *syncStatus;
    int intr;

    readyStatus = &D_8009B295;
    syncStatus = readyStatus - 1;
    savedMask = *D_8009B27C & 3;

    while ((intr = getintr()) != 0) {
        if (intr & 4) {
            if (D_8009AFB8 != 0) {
                D_8009AFB8(*readyStatus, D_800A3468);
            }
        }

        if (intr & 2) {
            if (D_8009AFB4 != 0) {
                D_8009AFB4(*syncStatus, D_800A3460);
            }
        }
    }

    *D_8009B27C = savedMask;
}
