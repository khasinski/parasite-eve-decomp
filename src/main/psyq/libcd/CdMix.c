typedef struct CdlATV {
    unsigned char val0;
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
} CdlATV;

extern void CD_vol(CdlATV *vol);

int CdMix(CdlATV *vol) {
    CD_vol(vol);
    return 1;
}
