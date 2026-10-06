/* GCC_VERSION: 2.8.1 */

#include "pe1/psyq_cd.h"
#include "pe1/cdrom.h"

CdlLOC *DsLastPos(CdlLOC *dst) {
    if (dst != 0) {
        *dst = *DS_lastpos();
        return dst;
    }

    return DS_lastpos();
}
