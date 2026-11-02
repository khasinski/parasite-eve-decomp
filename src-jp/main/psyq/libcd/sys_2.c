/* ASSEMBLER: GNU */
/* Psy-Q LIBCD SYS.OBJ, part 2 of 2: CdFlush, CdSetDebug, CdComstr, CdIntstr,
 * CdSync, CdReady, CdSyncCallback, CdReadyCallback, CdControl, CdControlF,
 * CdControlB, CdMix, CdGetSector, CdGetSector2, CdDataCallback, CdDataSync,
 * CdIntToPos and CdPosToInt. */

#include "pe1/psyq_cd.h"

#include "sys_2_externs.h"

void CdFlush(void) {
    CD_flush();
}

int CdSetDebug(int level) {
    int old = CD_debug;
    CD_debug = level;
    return old;
}

const char *CdComstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 0x1C) {
        return g_CdNoneString;
    }
    return g_CdCommandNames[index];
}

const char *CdIntstr(unsigned int index) {
    index &= 0xFF;
    if (index >= 7) {
        return g_CdNoneString;
    }
    return g_CdInterruptNames[index];
}

int CdSync(int mode, u8 *result) {
    return CD_sync(mode, result);
}

int CdReady(int mode, u8 *result) {
    return CD_ready(mode, result);
}

CdlCB CdSyncCallback(CdlCB callback) {
    CdlCB old;

    old = g_CdSyncCallback;
    g_CdSyncCallback = callback;
    return old;
}

CdlCB CdReadyCallback(CdlCB callback) {
    CdlCB old;

    old = g_CdReadyCallback;
    g_CdReadyCallback = callback;
    return old;
}

u32 g_CdCommandHasParam[32] = {
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
    tries = cmd;
    cmd_reg = tries;
    tries = 3;
    cmd_byte = cmd_reg & 0xFF;
    table = g_CdCommandHasParam;

    saved = g_CdSyncCallback;
    slot = &table[cmd_byte];
    ret = 0;

    minus_one = -1;

    do {
        g_CdSyncCallback = 0;
        one = 1;
        if (cmd_byte != one && (*(u8 *)&g_CdStatus & 0x10) != 0) {
            register int command = 1;
            register void *payload;
            register u8 *result;

            payload = 0;
            result = 0;
            CD_cw(command, payload, result, 0);
        }
        if (param_reg != 0 && slot[0] != 0) {
            if (CD_cw(2, param_reg, extra_reg, 0) != 0) {
                continue;
            }
        }
        g_CdSyncCallback = saved;
        if (CD_cw(cmd_reg & 0xFF, param_reg, extra_reg, 0) != 0) {
            continue;
        }
        return ret + 1;

    } while (--tries != minus_one);
    g_CdSyncCallback = saved;
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

    saved_callback = g_CdSyncCallback;
    retries = command;
    parameter = parameters;
    command_word = retries;
    retries = 3;
    one = 1;
    command_byte = command_word & 0xFF;
    flag_table = g_CdCommandHasParam;
    command_flags = &flag_table[command_byte];
    status = 0;
    exhausted = -1;

    do {
        g_CdSyncCallback = 0;
        if (command_byte != one && (*(u8 *)&g_CdStatus & 0x10) != 0) {
            CD_cw(1, 0, 0, 0);
        }
        if (parameter != 0 && command_flags[0] != 0 &&
            CD_cw(2, parameter, 0, 0) != 0) {
            continue;
        }
        g_CdSyncCallback = saved_callback;
        if (CD_cw(command_word & 0xFF, parameter, 0, 1) == 0) {
            goto done;
        }
    } while (--retries != exhausted);

    g_CdSyncCallback = saved_callback;
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

        previous_callback = g_CdSyncCallback;
        retries = 3;
        one = 1;
        command_byte = command & 0xFF;
        table = g_CdCommandHasParam;
        slot = &table[command_byte];
        retry_end = -1;
        for (; retries != retry_end; --retries) {
            g_CdSyncCallback = 0;

            if (command_byte != one && (*(u8 *)&g_CdStatus & 0x10) != 0) {
                CD_cw(1, 0, 0, 0);
            }

            if (parameter == 0 || slot[0] == 0 ||
                CD_cw(2, parameter, result, 0) == 0) {
                g_CdSyncCallback = previous_callback;
                if (CD_cw(command & 0xFF, parameter, result, 0) == 0) {
                    return 0;
                }
            }
        }

        g_CdSyncCallback = previous_callback;
        return -1;
    }

    if (retry_command(command, parameter, result) != 0) {
        return 0;
    }
    return CD_sync(0, result) == 2;
}

int CdMix(CdlATV *volume) {
    CD_vol(volume);
    return 1;
}

int CdGetSector(void *address, int size) {
    return CD_getsector(address, size) == 0;
}

int CdGetSector2(void *address, int size) {
    return CD_getsector2(address, size) == 0;
}

DsCallback CdDataCallback(DsCallback callback) {
    return DMACallback(3, callback);
}

int CdDataSync(int mode) {
    return CD_datasync(mode);
}


static inline int ENCODE_BCD(int value) {
    return ((value / 10) << 4) + (value % 10);
}

CdlLOC *CdIntToPos(int sector, CdlLOC *position) {
    sector += 150;
    position->sector = ENCODE_BCD(sector % 75);
    position->second = ENCODE_BCD(sector / 75 % 60);
    position->minute = ENCODE_BCD(sector / 75 / 60);
    return position;
}

#define DECODE_BCD(value) (((value) >> 4) * 10 + ((value) & 0xF))

int CdPosToInt(CdlLOC *position) {
    u_char sector = position->sector;
    u_char second = position->second;
    u_char minute = position->minute;

    return (DECODE_BCD(minute) * 60 + DECODE_BCD(second)) * 75 +
           DECODE_BCD(sector) - 150;
}
