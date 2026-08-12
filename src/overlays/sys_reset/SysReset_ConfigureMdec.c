#include "common.h"
#include "pe1/sys_reset.h"


void func_8010C0FC(int mode) {
    switch (mode) {
    case 0:
        *D_8010DB50 = 0x80000000;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        *D_8010DB50 = 0x60000000;
        func_8010C1EC(D_8010DA0C, 0x20);
        func_8010C1EC(D_8010DA90, 0x20);
        break;
    case 1:
        *D_8010DB50 = 0x80000000;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        (void)*D_8010DB30;
        *D_8010DB50 = 0x60000000;
        break;
    default:
        func_80071A74(D_8010BCFC, mode);
        break;
    }
}
