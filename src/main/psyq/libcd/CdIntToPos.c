/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBCD SYS.OBJ: CdIntToPos, CdPosToInt. */
#include "pe1/psyq_cd.h"

static inline int ENCODE_BCD(int value) {
    return ((value / 10) << 4) + (value % 10);
}

CdlLOC *CdIntToPos(int sector, CdlLOC *position) {
    sector += 150;
    position->sector = ENCODE_BCD(sector % 75);
    position->second = ENCODE_BCD(sector / 75 % 60);
    position->minute = ENCODE_BCD(sector / 75 / 60);
    return position;
}

#define DECODE_BCD(value) (((value) >> 4) * 10 + ((value) & 0xF))

int CdPosToInt(CdlLOC *position) {
    u_char sector = position->sector;
    u_char second = position->second;
    u_char minute = position->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
