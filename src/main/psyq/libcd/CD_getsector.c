/* MASPSX_FLAGS: --stack-return-delay */

typedef unsigned char u8;
typedef unsigned int u32;

extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegResponse;
extern volatile u32 *g_CdRegRequest;
extern volatile u32 *D_8009B2B0;
extern volatile u32 *D_8009B2B4;
extern volatile u32 *D_8009B2B8;
extern volatile u32 *D_8009B2BC;
extern volatile u32 *g_CdRegDmaControl;

int CD_getsector(u32 arg0, u32 arg1) {
    volatile u32 readback;
    volatile u8 *status;

    *g_CdRegIndexBase = 0;
    *g_CdRegResponse = 0x80;
    *D_8009B2B0 = 0x21020843;
    *g_CdRegRequest = 0x1325;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = arg0;
    *D_8009B2BC = arg1 | 0x10000;

    status = g_CdRegIndexBase;
    if ((*status & 0x40) == 0) {
        do {
        } while ((*status & 0x40) == 0);
    }

    *g_CdRegDmaControl = 0x11400100;
    readback = *g_CdRegDmaControl;

    goto done;

done:
    return 0;
}
