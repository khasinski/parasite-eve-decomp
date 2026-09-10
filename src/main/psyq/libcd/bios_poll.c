/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-expensive-optimizations */

#include "pe1/psyq_cd.h"
#include "pe1/psyq_bios.h"

/* Adjacent command/ready polling routines from LIBCD BIOS_1.OBJ. */
/* Compare the previous poll count, then commit the incremented counter. */
static inline int count_poll(void) {
    int count = D_800A347C;
    int old = count++;
    D_800A347C = count;
    return old;
}

static inline int timed_out(char **commands, char **events, u8 *sync) {
    if (VSync(-1) > D_800A3478 || count_poll() > 0x3C0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[D_8009AFD5],
               events[((volatile u8 *)sync)[0]], events[((volatile u8 *)sync)[1]]);
        CD_flush();
        return -1;
    }
    return 0;
}

static inline void copy_result(u8 *destination, const volatile u8 *source) {
    int remaining;
    if (destination) {
        remaining = 7;
        for (; remaining != -1; remaining--) {
            *destination++ = *source++;
        }
    }
}

static inline void dispatch_interrupts(u8 *sync, u8 *ready) {
    int bank = *D_8009B27C & 3;
    int pending;
    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8)
            D_8009AFB8(*ready, D_800A3468);
        if ((pending & 2) && D_8009AFB4)
            D_8009AFB4(*sync, D_800A3460);
    }
    *D_8009B27C = bank;
}

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
        if (timed_out(commands, events, sync)) {
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
        if (timed_out(commands, events, sync)) {
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
