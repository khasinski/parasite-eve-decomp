/* MASPSX_FLAGS: --stack-return-delay */

#include "pe1/psyq_gpu.h"

typedef unsigned int u32;

#include "pe1/gpu_callbacks.h"

extern char D_800118EC[];
extern u32 D_800957EC[];
extern GpuCallbacks *g_GpuCallbacks;

void checkRECT(char *msg, RECT *rect);

int MoveImage(RECT *rect, int x, int y) {
    register u32 *packet;
    GpuCallbacks *callbacks;
    int dst;
    u32 xy;
    u32 low;
    u32 command;
    u32 wh;

    checkRECT(D_800118EC, rect);
    if (rect->w == 0) {
        return -1;
    }
    if (rect->h != 0) {
        dst = y << 16;
        asm volatile("" : "=r"(packet) : "0"(D_800957EC), "r"(dst));
        low = x & 0xFFFF;
        command = dst | low;
        xy = *(u32 *)&rect->x;
        callbacks = g_GpuCallbacks;
        packet[1] = command;
        packet[0] = xy;
        wh = *(u32 *)&rect->w;
        packet[2] = wh;
        return callbacks->addque2(callbacks->u18.cwc, packet - 2, 0x14, 0);
    }

    return -1;
}
