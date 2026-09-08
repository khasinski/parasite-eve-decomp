void DMACallback(int channel, int callback);

void CdDataCallback(int callback) {
    DMACallback(3, callback);
}
void CD_datasync(void);

void CdDataSync(void) {
    CD_datasync();
}
#include "pe1/psyq_cd.h"

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
#include "pe1/psyq_cd.h"

int CdPosToInt_Local(CdlLOC *p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
    u_char sector = p->sector;
    u_char second = p->second;
    u_char minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
