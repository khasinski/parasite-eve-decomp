/* GCC_VERSION: 2.8.1 */
#include "pe1/psyq_cd.h"

int func_8007A740(int command, void *parameter, u8 *result) {
    int retries = 3;
    int status = 0;
    unsigned int code = (u8)command;
    CdlCB saved = D_8009AFB4;
    u32 *needs_location = &D_8009AF2C[code];

    do {
        D_8009AFB4 = 0;
        if (code != 1 && (*(u8 *)&D_8009AFC4 & 0x10))
            CD_cw(1, 0, 0, 0);
        if (parameter && *needs_location && CD_cw(2, parameter, result, 0))
            continue;
        D_8009AFB4 = saved;
        if (CD_cw((u8)command, parameter, result, 0) == 0)
            goto done;
    } while (--retries != -1);
    D_8009AFB4 = saved;
    status = -1;
done:
    if (status == 0) return CD_sync(0, result) == 2;
    return 0;
}
