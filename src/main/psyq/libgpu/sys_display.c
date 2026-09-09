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

#include "include_asm.h"
#include "pe1/gpu_state.h"

extern char D_80011884[];

int DrawSync(int arg0) {
    register int saved asm("$16");
    void (*fn)(char *, int);
    GpuCallbacks *callbacks;

    saved = arg0;
    if (D_8009574C.queueState.debugLevel >= 2) {
        fn = D_80095748;
        fn(D_80011884, saved);
    }

    callbacks = D_80095744;
    callbacks->callback3C(saved);
}

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"

extern char D_800118B8[], D_800118A4[];

void checkRECT(char *name, RECT *rect) {
    char *format;
    switch (D_8009574C.queueState.debugLevel) {
    case 1:
        if (rect->w > D_8009574C.width || rect->w + rect->x > D_8009574C.width ||
            rect->y > D_8009574C.height || rect->y + rect->h > D_8009574C.height ||
            rect->w <= 0 || rect->x < 0 || rect->y < 0 || rect->h <= 0) {
            /* USA D_80011898: keep page and string offset separate. */
            format = (char *)0x80010000;
            asm volatile("" : "=r"(format) : "0"(format));
            format += 0x1898;
            break;
        }
        return;
    case 2:
        format = D_800118B8;
        break;
    default:
        return;
    }
    D_80095748(format, name);
    {
        int x = rect->x;
        int y = rect->y;
        int width = rect->w;
        register int height asm("$3") = rect->h;
        D_80095748(D_800118A4, x, y, width, height);
    }
}

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"

extern char D_800118BC[];
extern char D_800118C8[];

void checkRECT(char *message, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118BC, rect);
    color = ((u32)b << 16) | ((u32)g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

int ClearImage2(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118C8, rect);
    color = 0x80000000 | ((u32)b << 16) | ((u32)g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"

extern char D_800118D4[];
extern char D_800118E0[];
extern char D_800118EC[];
extern GpuCallbacks *D_80095744;
extern u32 D_800957EC[];

void checkRECT(char *message, RECT *rect);

int LoadImage(RECT *rect, void *pixels) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118D4, rect);
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->u20.load, rect, 8, pixels);
}

int StoreImage(RECT *rect, void *pixels) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118E0, rect);
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->u1c.store, rect, 8, pixels);
}

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
