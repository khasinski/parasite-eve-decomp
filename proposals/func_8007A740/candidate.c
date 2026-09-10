#include "pe1/psyq_cd.h"

int func_8007A740(u_char command, u_char *parameter, u_char *result) {
    static inline int retry_command(u_char command, u_char *parameter,
                                    u_char *result) {
        int retries;
        int retry_end = -1;
        CdlCB previous_callback = D_8009AFB4;

        for (retries = 3; retries != retry_end; --retries) {
            D_8009AFB4 = 0;

            if (command != 1 && (*(u8 *)&D_8009AFC4 & 0x10) != 0) {
                CD_cw(1, 0, 0, 0);
            }

            if (parameter == 0 || D_8009AF2C[command] == 0 ||
                CD_cw(2, parameter, result, 0) == 0) {
                D_8009AFB4 = previous_callback;
                if (CD_cw(command, parameter, result, 0) == 0) {
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
