/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
int func_8007A60C(int command, void *parameter) {
    int retries = 3;
    int status = 0;
    unsigned int code = (unsigned char)command;
    int saved = D_8009AFB4;
    u32 *needs_location = &D_8009AF2C[code];
    do {
        D_8009AFB4 = 0;
        if (code != 1 && (D_8009AFC4 & 0x10))
            CD_cw(1, 0, 0, 0);
        if (parameter && *needs_location && CD_cw(2, parameter, 0, 0))
            continue;
        D_8009AFB4 = saved;
        if (!CD_cw((unsigned char)command, parameter, 0, 1))
            return status + 1;
    } while (--retries != -1);
    D_8009AFB4 = saved;
    status = -1;
    return status + 1;
}
