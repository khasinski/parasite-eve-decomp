/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "pe1/psyq_cd.h"

int func_8007A4D0(int cmd, void *param, u8 *extra) {
    register int tries;
    register void *param_reg;
    register u8 *extra_reg;
    register int cmd_reg;
    register int cmd_byte;
    register CdlCB saved asm("$21");
    register u32 *slot;
    register int ret asm("$23");
    register int minus_one;
    register int one asm("$8");
    register u32 *table;

    param_reg = param;
    extra_reg = extra;
    cmd_reg = cmd;
    tries = 3;
    cmd_byte = cmd_reg & 0xFF;
    table = D_8009AF2C;

    saved = D_8009AFB4;
    slot = &table[cmd_byte];
    ret = 0;

    minus_one = -1;

    do {
        D_8009AFB4 = 0;
        one = 1;
        asm volatile("" : "+r"(one));
        if (cmd_byte != one && (D_8009AFC4 & 0x10) != 0) {
            register int command = 1;
            register void *payload;
            register u8 *result;

            /* Keep independent zero arguments and the retail argument order. */
            asm volatile("" : "+r"(command));
            payload = 0;
            asm volatile("" : "+r"(payload));
            result = 0;
            asm volatile("" : "+r"(result));
            CD_cw(command, payload, result, 0);
        }
        if (param_reg != 0 && slot[0] != 0) {
            if (CD_cw(2, param_reg, extra_reg, 0) != 0) {
                continue;
            }
        }
        D_8009AFB4 = saved;
        if (CD_cw(cmd_reg & 0xFF, param_reg, extra_reg, 0) != 0) {
            continue;
        }
        return ret + 1;

    } while (--tries != minus_one);
    D_8009AFB4 = saved;
    ret = -1;
    asm volatile("" : "+r"(ret));
    return ret + 1;
}
