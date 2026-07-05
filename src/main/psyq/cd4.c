/* MASPSX_FLAGS: --store-return-delay */
#include "include_asm.h"

extern void CD_sync(void);

extern void CD_ready(void);

extern int g_CdSyncCallback;

extern int g_CdReadyCallback;

typedef struct CdlATV {
    unsigned char val0;
    unsigned char val1;
    unsigned char val2;
    unsigned char val3;
} CdlATV;

extern void CD_vol(CdlATV *vol);

extern int CD_getsector2(void);

extern int CD_getsector(void);

void DMACallback(int arg0, int arg1);

extern void CD_datasync(void);

#include "pe1/psyq_cd.h"

void CdSync(void) {
    CD_sync();
}

void CdReady(void) {
    CD_ready();
}

int CdSyncCallback(int callback) {
    int old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}

int CdReadyCallback(int callback) {
    int old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/cd4", func_8007A4D0);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/cd4", func_8007A60C);

INCLUDE_ASM("asm/USA/main/nonmatchings/psyq/cd4", func_8007A740);

int CdMix(CdlATV *vol) {
    CD_vol(vol);
    return 1;
}

int CdGetSector2(void) {
    return CD_getsector2() == 0;
}

int CdGetSector(void) {
    return CD_getsector() == 0;
}

void CdDataCallback(int arg) {
    DMACallback(3, arg);
}

void CdDataSync(void) {
    CD_datasync();
}

static inline int ENCODE_BCD(int n) {
    return ((n / 10) << 4) + (n % 10);
}

CdlLOC *CdIntToPos_Local(int i, CdlLOC *p) {
    i += 150;
    p->sector = ENCODE_BCD(i % 75);
    p->second = ENCODE_BCD(i / 75 % 60);
    p->minute = ENCODE_BCD(i / 75 / 60);
    return p;
}

int CdPosToInt_Local(CdlLOC *p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
    u_char sector = p->sector;
    u_char second = p->second;
    u_char minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
