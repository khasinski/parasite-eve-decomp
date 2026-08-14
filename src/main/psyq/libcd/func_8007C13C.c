/* AS_MODE: reorder */
/* CC1_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

extern volatile unsigned char *g_CdRegIndexBase;
extern unsigned char D_8009B295;
extern void (*g_CdReadyCallback)(unsigned char, unsigned char *);
extern void (*g_CdSyncCallback)(unsigned char, unsigned char *);
extern unsigned char D_800A3460[];
extern unsigned char D_800A3468[];

int getintr(void);

void Cd_SetIntrMask(void) {
    unsigned char *readyStatus;
    unsigned char *syncStatus;
    unsigned char savedMask;
    int intr;

    readyStatus = &D_8009B295;
    syncStatus = readyStatus - 1;
    savedMask = *g_CdRegIndexBase & 3;

    while ((intr = getintr()) != 0) {
        if (intr & 4) {
            if (g_CdReadyCallback != 0) {
                g_CdReadyCallback(*readyStatus, D_800A3468);
            }
        }

        if (intr & 2) {
            if (g_CdSyncCallback != 0) {
                g_CdSyncCallback(*syncStatus, D_800A3460);
            }
        }
    }

    *g_CdRegIndexBase = savedMask;
}
