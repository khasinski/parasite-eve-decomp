/* CC1_PROFILE: build0001 */
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
