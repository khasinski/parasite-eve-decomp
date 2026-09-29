/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-expensive-optimizations */

#include "bios_internal.h"

/* Adjacent command/ready polling routines from LIBCD BIOS_1.OBJ. */

int CD_sync(int mode, u8 *result) {
    u8 *sync;
    u8 *ready;
    char **commands;
    char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = D_8009AFDC;
    events = D_8009B05C;
    sync = (u8 *)&D_8009B294;
    ready = sync + 1;
    D_800A347C = 0;
    D_800A3480 = D_80011BA0;
    do {
        int status;
        if (timed_out(commands, events, (CdInterruptEvents *)sync)) {
            return -1;
        }
        if (CheckCallback()) {
            dispatch_interrupts(sync, ready);
        }
        status = ((volatile u8 *)sync)[0];
        if (status == 2 || status == 5) {
            ((volatile u8 *)sync)[0] = 2;
            copy_result(result, D_800A3460);
            return status;
        }
    } while (!mode);
    return 0;
}

int CD_ready(int mode, u8 *result) {
    u8 *sync;
    u8 *ready;
    volatile u8 *end;
    char **commands;
    char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = D_8009AFDC;
    events = D_8009B05C;
    sync = (u8 *)&D_8009B294;
    ready = sync + 1;
    end = sync + 2;
    D_800A347C = 0;
    D_800A3480 = D_80011BA8;
    do {
        int status;
        if (timed_out(commands, events, (CdInterruptEvents *)sync)) {
            return -1;
        }
        if (CheckCallback()) {
            dispatch_interrupts(sync, ready);
        }
        status = *end;
        if (status) {
            ((volatile u8 *)sync)[2] = 0;
            copy_result(result, D_800A3470);
            return status;
        }
        status = end[-1];
        if (status) {
            ((volatile u8 *)sync)[1] = 0;
            copy_result(result, D_800A3468);
            return status;
        }
    } while (!mode);
    return 0;
}
