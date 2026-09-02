/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"

extern char D_800118EC[];
extern GpuCallbacks *D_80095744;
extern u32 D_800957EC[];

void checkRECT(char *message, RECT *rect);

int MoveImage(RECT *rect, int x, int y) {
    checkRECT(D_800118EC, rect);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    {
        register u32 *packetData asm("$3");
        GpuCallbacks *callbacks;
        register u32 sourcePosition asm("$5");
        u32 *packet;
        int packetSize;
        u32 destinationPosition;
        register u32 destinationX asm("$4");
        u32 size;

        destinationPosition = (u32)y << 16;
        packetData = D_800957EC;
        asm("" : "+r"(packetData));
        destinationX = x & 0xFFFF;
        destinationPosition |= destinationX;
        sourcePosition = *(u32 *)rect;
        callbacks = D_80095744;
        asm("" : "+r"(sourcePosition), "+r"(callbacks));
        packetSize = 20;
        asm("" : "+r"(packetSize));
        packetData[1] = destinationPosition;
        *packetData = sourcePosition;
        size = *((u32 *)rect + 1);
        packet = packetData - 2;
        packetData[2] = size;
        asm("" : : "r"(size) : "memory");
        return callbacks->addque2(callbacks->u18.moveImage, packet, packetSize,
                                  0);
    }
}
