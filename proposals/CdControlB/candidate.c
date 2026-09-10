/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses -fno-schedule-insns */

#include "pe1/psyq_cd.h"

int CdControlB_Impl(int cmd, void *param, u8 *result) __asm__("CdControlB");

int CdControlB_Impl(int cmd, void *param, u8 *result) {
    register int tries;
    register void *param_reg;
    register u8 *result_reg;
    register int cmd_reg;
    register int cmd_byte;
    register CdlCB saved;
    register u32 *slot;
    register int minus_one;
    register u32 *table;
    int command_result;

    param_reg = param;
    result_reg = result;
    cmd_reg = cmd;
    tries = 3;
    cmd_byte = cmd_reg & 0xFF;
    table = D_8009AF2C;
    saved = D_8009AFB4;
    slot = &table[cmd_byte];
    minus_one = -1;

    do {
        D_8009AFB4 = 0;
        if (cmd_byte != 1 && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
            CD_cw(1, 0, 0, 0);
        }
        if (param_reg != 0 && slot[0] != 0) {
            command_result = CD_cw(2, param_reg, result_reg, 0);
            if (command_result != 0) {
                continue;
            }
        }
        D_8009AFB4 = saved;
        command_result = CD_cw(cmd_reg & 0xFF, param_reg, result_reg, 0);
        if (command_result != 0) {
            continue;
        }
        break;
    } while (--tries != minus_one);

    if (command_result != 0) {
        D_8009AFB4 = saved;
        return 0;
    }
    return CD_sync(0, result_reg) == 2;
}
