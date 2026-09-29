/* GCC_VERSION: 2.8.1 */

#include "pe1/cdrom.h"
#include "pe1/psyq_cd.h"

extern int VSync(int arg0);
extern void Save_ProcessDataCallback(void);

int Sys_VSyncTimeout(void *argument) {
    int v0;
    int s0;
    int *state;

    v0 = VSync(-1);
    state = &g_CdReadStartVsync;
    asm volatile("" : "=r"(state) : "0"(state));

    if ((state[0] + 0x4B0) < v0) {
        Save_ProcessDataCallback();
        s0 = -1;
    } else {
        s0 = state[-4];
    }

    CdRom_SendQueuedCmd(argument);
    return s0;
}
