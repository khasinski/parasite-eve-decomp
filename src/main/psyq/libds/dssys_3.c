/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_3.OBJ: DsMix, DsGetSector, DsGetSector2, DsDataSync, DsIntToPos, DsPosToInt, DsSetDebug, DsLastPos. */
#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

extern void CD_vol(void);

extern int CD_getsector(void);

extern int CD_getsector2(void);

int DsMix(void) {
    CD_vol();
    return 1;
}

int DsGetSector(void) {
    return CD_getsector() == 0;
}

int DsGetSector2(void) {
    return CD_getsector2() == 0;
}

int DsDataSync(int mode) {
    return CD_datasync(mode);
}

static inline int ENCODE_BCD(int n) { return ((n / 10) << 4) + (n % 10); }

CdlLOC *DsIntToPos(int i, CdlLOC *p) {
    i += 150;
    p->sector = ENCODE_BCD(i % 75);
    p->second = ENCODE_BCD(i / 75 % 60);
    p->minute = ENCODE_BCD(i / 75 / 60);
    return p;
}

int DsPosToInt(CdlLOC *p) {
#define DECODE_BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))
    u_char sector = p->sector;
    u_char second = p->second;
    u_char minute = p->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}

int DsSetDebug(int level) {
    int old = CD_debug;
    CD_debug = level;
    return old;
}

CdlLOC *DsLastPos(CdlLOC *dst) {
    if (dst != 0) {
        *dst = *DS_lastpos();
        return dst;
    }

    return DS_lastpos();
}
