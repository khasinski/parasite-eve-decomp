#include "common.h"
#include "pe1/sys_reset.h"


void func_8010C1EC(s32 *data, u32 size) {
    func_8010C308();
    *D_8010DB54 |= 0x88;
    *D_8010DB1C = data + 1;
    *D_8010DB20 = ((size >> 5) << 16) | 0x20;
    *D_8010DB4C = data[0];
    *D_8010DB24 = 0x1000201;
}
