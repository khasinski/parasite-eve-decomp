/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "pe1/psyq_cd.h"

int CdControlF_Impl(int cmd, void *param) __asm__("CdControlF");

int CdControlF_Impl(int cmd, void *param) {
    register int tries;
    register void *param_reg;
    int one;
    register int cmd_reg;
    register int cmd_byte;
    register CdlCB saved asm("$20");
    register u32 *slot;
    register int ret asm("$22");
    register int minus_one;
    register u32 *table;

    param_reg = param;
    cmd_reg = cmd;
    tries = 3;
    one = 1;
    cmd_byte = cmd_reg & 0xFF;
    table = D_8009AF2C;
    saved = D_8009AFB4;
    slot = &table[cmd_byte];
    ret = 0;
    minus_one = -1;

    do {
        int call_result;

        D_8009AFB4 = 0;
        if (cmd_byte != one && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
            CD_cw(1, 0, 0, 0);
        }
        if (param_reg != 0 && slot[0] != 0) {
            if (CD_cw(2, param_reg, 0, 0) != 0) {
                continue;
            }
        }
        D_8009AFB4 = saved;
        call_result = CD_cw(cmd_reg & 0xFF, param_reg, 0, 1);
        asm volatile("" : : "r"(one));
        if (call_result != 0) {
            continue;
        }
        return ret + 1;
    } while (--tries != minus_one);

    D_8009AFB4 = saved;
    ret = -1;
    asm volatile("" : "+r"(ret) : "r"(one));
    return ret + 1;
}
