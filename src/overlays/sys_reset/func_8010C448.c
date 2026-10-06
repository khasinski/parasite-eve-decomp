#include "pe1/sys_reset.h"

int func_8010C448(char *message) {
    func_80071A74(D_8010BD38, message);
    *D_8010DB50 = 0x80000000;
    *D_8010DB24 = 0;
    *D_8010DB30 = 0;
    *D_8010DB30;
    *D_8010DB50 = 0x60000000;
    return 0;
}
