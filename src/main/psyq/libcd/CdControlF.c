/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-schedule-insns */
#include "pe1/psyq_cd.h"

/* The public API takes a byte command; the implementation keeps its incoming
 * argument word and masks it at the command-table lookup and submission. */
int CdControlF_Impl(int command, void *parameters) __asm__("CdControlF");

int CdControlF_Impl(int command, void *parameters) {
    int retries;
    void *parameter;
    int one;
    int command_word;
    int command_byte;
    CdlCB saved_callback;
    u32 *command_flags;
    int status;
    int exhausted;
    u32 *flag_table;

    saved_callback = D_8009AFB4;
    parameter = parameters;
    command_word = command;
    retries = 3;
    one = 1;
    command_byte = command_word & 0xFF;
    flag_table = D_8009AF2C;
    command_flags = &flag_table[command_byte];
    status = 0;
    exhausted = -1;

    do {
        D_8009AFB4 = 0;
        if (command_byte != one && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
            CD_cw(1, 0, 0, 0);
        }
        if (parameter != 0 && command_flags[0] != 0 &&
            CD_cw(2, parameter, 0, 0) != 0) {
            continue;
        }
        D_8009AFB4 = saved_callback;
        if (CD_cw(command_word & 0xFF, parameter, 0, 1) == 0) {
            goto done;
        }
    } while (--retries != exhausted);

    D_8009AFB4 = saved_callback;
    status = -1;
done:
    return status + 1;
}
