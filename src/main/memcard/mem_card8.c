/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/card_obj.h"

extern int g_MemCardPort1Present;
extern int g_MemCardPort2Present;
extern int D_800A5AC0;
extern int D_800A5AC4;
extern int g_MemCardServiceReady;
extern CardObj *g_MemCardObjArray;
extern void (*g_MemCardObjResetFn)(CardObj *obj);

int mem_card8(int portMask) {
    int currentMask;
    register int timerValue asm("$2");
    register int *timer1 asm("$18");
    int *timer2;
    currentMask = (g_MemCardPort2Present << 1)
        | (g_MemCardPort1Present == 0);
    if (currentMask != portMask) {
        g_MemCardServiceReady = 0;

        if (portMask & 1) {
            timer1 = &D_800A5AC0;
            timerValue = *timer1;
            g_MemCardPort1Present = 0;
            if (timerValue >= 150) {
                g_MemCardObjResetFn(g_MemCardObjArray);
            }
            *timer1 = 0;
        } else {
            g_MemCardPort1Present = 1;
        }

        if (portMask & 2) {
            timer2 = &D_800A5AC4;
            timerValue = *timer2;
            g_MemCardPort2Present = 1;
            if (timerValue >= 150) {
                g_MemCardObjResetFn(g_MemCardObjArray + 1);
            }
            *timer2 = 0;
        } else {
            g_MemCardPort2Present = 0;
        }

        g_MemCardServiceReady = 1;
    }
    return currentMask;
}
