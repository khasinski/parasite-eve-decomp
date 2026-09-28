/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-schedule-insns */

#include "pe1/psyq_cd.h"

u32 D_8009AF2C[32] = {
    0, 0, 0, 1, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 0,
};

/* LIBCD.H exposes u_char/pointer arguments, while S_016 was compiled from an
 * internal definition that still treats the command as an promoted int. */
int CdControl_Impl(int cmd, void *param, u8 *extra) __asm__("CdControl");

int CdControl_Impl(int cmd, void *param, u8 *extra) {
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
        asm volatile("" : "=r"(one) : "0"(one));
        if (cmd_byte != one && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
            register int command = 1;
            register void *payload;
            register u8 *result;

            /* Keep independent zero arguments and the retail argument order. */
            asm volatile("" : "=r"(command) : "0"(command));
            payload = 0;
            asm volatile("" : "=r"(payload) : "0"(payload));
            result = 0;
            asm volatile("" : "=r"(result) : "0"(result));
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
    asm volatile("" : "=r"(ret) : "0"(ret));
    return ret + 1;
}
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
int CdControlB(u_char command, u_char *parameter, u_char *result) {
    static inline int retry_command(u_char command, u_char *parameter,
                                   u_char *result) {
        int retries;
        int one;
        int retry_end;
        int command_byte;
        u32 *slot;
        u32 *table;
        CdlCB previous_callback;

        previous_callback = D_8009AFB4;
        retries = 3;
        one = 1;
        command_byte = command & 0xFF;
        table = D_8009AF2C;
        slot = &table[command_byte];
        retry_end = -1;
        for (; retries != retry_end; --retries) {
            D_8009AFB4 = 0;

            if (command_byte != one && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
                CD_cw(1, 0, 0, 0);
            }

            if (parameter == 0 || slot[0] == 0 ||
                CD_cw(2, parameter, result, 0) == 0) {
                D_8009AFB4 = previous_callback;
                if (CD_cw(command & 0xFF, parameter, result, 0) == 0) {
                    return 0;
                }
            }
        }

        D_8009AFB4 = previous_callback;
        return -1;
    }

    if (retry_command(command, parameter, result) != 0) {
        return 0;
    }
    return CD_sync(0, result) == 2;
}
