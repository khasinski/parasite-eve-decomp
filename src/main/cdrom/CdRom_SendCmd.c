/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_cd.h"
extern int g_CdRomCmdLongTimeoutTable[];
extern void CD_flush(void);
extern void Util_Copy4(void *, const void *);
extern int CD_cw(int, void *, int, int);

int CdRom_SendCmd(unsigned char command, void *param) {
    unsigned char *state;
    int *timeout;
    CD_flush();
    state = (unsigned char *)&g_CdRomEventCommandState;
    state[0] = command;
    if (param) {
        unsigned char *copy = state + 1;
        Util_Copy4(copy, param);
        *(void **)(state + 8) = copy;
    } else *(void **)(state + 8) = 0;
    timeout = &g_CdRomCmdTimeout;
    timeout[0] = g_CdRomCmdLongTimeoutTable[((unsigned char *)timeout)[-64]] ? 960 : 30;
    timeout[1] = 0;
    switch (((unsigned char *)timeout)[-64]) {
    case 7:
        if (((unsigned char *)timeout)[-13] == 1) {
            ((unsigned char *)timeout)[-64] = 1;
            timeout[-14] = 0;
        }
        break;
    case 8:
        if (((unsigned char *)timeout)[-13] != 1) {
            ((unsigned char *)timeout)[-64] = 1;
            timeout[-14] = 0;
        }
        break;
    }
    {
        unsigned char *current = (unsigned char *)&g_CdRomEventCommandState;
        if (CD_cw(current[0], *(void **)(current + 8), 0, 1)) {
            *(int *)(current + 68) = 0;
            *(int *)(current + 64) = 0;
            return 0;
        } else {
            current[40] = current[0];
            return 1;
        }
    }
}
