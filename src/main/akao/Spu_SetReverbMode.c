
/* CC1_FLAGS: -fno-schedule-insns */
/* CC1_FLAGS: -fno-schedule-insns2 */

extern int D_8009B38C;

typedef struct SpuReverbDataPage {
    int enabled;
    char reserved04[0x4BE4];
} SpuReverbDataPage;

register SpuReverbDataPage *g_SpuReverbWritePage asm("$1");

int Spu_SetReverbMode(int mode) {
    int enabled;

    switch (mode) {
    case 0:
        enabled = 0;
        break;
    case 1:
        enabled = 1;
        break;
    default:
        enabled = 0;
        break;
    }

    D_8009B38C = mode;
    g_SpuReverbWritePage = (SpuReverbDataPage *)0x800A0000;
    g_SpuReverbWritePage[-1].enabled = enabled;
    return enabled;
}
