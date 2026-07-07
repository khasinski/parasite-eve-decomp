/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
/* MASPSX_FLAGS: --stack-return-delay */


#include "include_asm.h"
asm(".globl DrawSync");
asm("DrawSync = DrawSync");

#include "pe1/gpu_callbacks.h"

extern unsigned char g_GraphDebug[];
extern void (*g_GpuDebugPrintf[])(char *msg, int arg0);
extern GpuCallbacks * g_GpuCallbacks[];
#define g_GpuCallbacks (g_GpuCallbacks[0])
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
