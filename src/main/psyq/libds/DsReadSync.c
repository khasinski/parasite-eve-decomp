/* GCC_VERSION: 2.8.1 */
/* Psy-Q LIBDS DSREAD.OBJ: DsReadSync. */

#include "pe1/cdrom.h"
#include "pe1/psyq_cd.h"

extern int VSync(int arg0);
extern void DsReadBreak(void);

int DsReadSync(void *argument) {
    int v0;
    int s0;
    int *state;

    v0 = VSync(-1);
    state = &g_CdReadStartVsync;
    asm volatile("" : "=r"(state) : "0"(state));

    if ((state[0] + 0x4B0) < v0) {
        DsReadBreak();
        s0 = -1;
    } else {
        s0 = state[-4];
    }

    DsReady(argument);
    return s0;
}
