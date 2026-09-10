/* CC1_FLAGS: -fno-schedule-insns */
/* ASSEMBLER: GNU */
#include "pe1/psyq_cd.h"

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
