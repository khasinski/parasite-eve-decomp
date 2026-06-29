/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FLAGS: --stack-return-delay */

asm(".globl func_80074DC0");
asm("func_80074DC0 = DrawSync");

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, int arg0);
extern struct {
    char _[16];
} g_GpuCallbacks_o __asm__("g_GpuCallbacks");
#define g_GpuCallbacks (*(GpuCallbacks **)&g_GpuCallbacks_o)
extern char D_80011884[];

int DrawSync(int arg0) {
    register int saved asm("$16");
    void (*fn)(char *, int);
    GpuCallbacks *callbacks;

    saved = arg0;
    if (g_GraphDebug[0] >= 2) {
        fn = g_GpuDebugPrintf[0];
        fn(D_80011884, saved);
    }

    callbacks = g_GpuCallbacks;
    callbacks->callback3C(saved);
}
