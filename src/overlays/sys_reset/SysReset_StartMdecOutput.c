#include "common.h"
#include "pe1/sys_reset.h"

/* MASPSX_FLAGS: --stack-return-delay */

void func_8010C27C(s32 *data, u32 size) {
    func_8010C39C();
    *D_8010DB54 |= 0x88;
    *D_8010DB30 = 0;
    *D_8010DB28 = data;
    *D_8010DB2C = ((size >> 5) << 16) | 0x20;
    *D_8010DB30 = 0x1000200;
}
