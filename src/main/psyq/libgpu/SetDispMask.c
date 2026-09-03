/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"

extern char D_80011870[];
extern GpuCallbacks *g_GpuCallbacks[];
extern void (*g_GpuDebugPrintf[])(char *message, int argument);
extern u8 g_GraphDebug[];

void GPU_memset(void *destination, int value, int size);

void SetDispMask(int mask) {
    register int enable asm("$16") = mask;
    u8 *debug = g_GraphDebug;
    u8 *clearPointer;
    GpuCallbacks *callbacks;
    int command;

    if (*debug >= 2) {
        g_GpuDebugPrintf[0](D_80011870, enable);
    }

    clearPointer = debug + 0x6A;
    if (enable == 0) {
        GPU_memset(clearPointer, -1, 20);
    }

    command = 0x03000000;
    asm("" : "+r"(command));
    callbacks = g_GpuCallbacks[0];
    if (enable == 0) {
        command |= 1;
    } else {
        command = 0x03000000;
    }
    callbacks->callback10(command);
}
