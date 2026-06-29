typedef unsigned char u8;

typedef struct CdlATV {
    u8 val0;
    u8 val1;
    u8 val2;
    u8 val3;
} CdlATV;

extern u8 *g_CdRegIndexBase;
extern u8 *g_CdRegPort1;
extern u8 *g_CdRegDataWrite;
extern u8 *g_CdRegResponse;

int CD_vol(CdlATV *vol) {
    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = vol->val0;
    *g_CdRegResponse = vol->val1;
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = vol->val2;
    *g_CdRegDataWrite = vol->val3;
    *g_CdRegResponse = 0x20;
    return 0;
}
