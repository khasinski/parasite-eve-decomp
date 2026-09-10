/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "pe1/psyq_cd.h"

int func_8007A60C(int cmd, void *param) {
    register int tries;
    register void *param_reg;
    int no_result;
    register int cmd_reg;
    register int cmd_byte;
    register CdlCB saved asm("$20");
    register u32 *slot;
    register int ret asm("$22");
    register int minus_one;
    register int one asm("$8");
    register u32 *table;

    param_reg = param;
    cmd_reg = cmd;
    tries = 3;
    no_result = 0;
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
        if (cmd_byte != one && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
            register int command = 1;
            register void *payload;
            register u8 *result;

            asm volatile("" : "+r"(command));
            payload = 0;
            asm volatile("" : "+r"(payload));
            result = 0;
            asm volatile("" : "+r"(result));
            CD_cw(command, payload, result, 0);
        }
        if (param_reg != 0 && slot[0] != 0) {
            if (CD_cw(2, param_reg, 0, 0) != 0) {
                continue;
            }
        }
        D_8009AFB4 = saved;
        if (CD_cw(cmd_reg & 0xFF, param_reg, no_result, 1) != 0) {
            continue;
        }
        return ret + 1;
    } while (--tries != minus_one);

    D_8009AFB4 = saved;
    ret = -1;
    asm volatile("" : "+r"(ret));
    return ret + 1;
}
