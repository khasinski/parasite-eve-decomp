/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ, from the object start: ResetGraph, SetGraphDebug,
 * SetGraphQueue, GetGraphDebug, DrawSyncCallback, SetDispMask, DrawSync,
 * ClearImage, ClearImage2, LoadImage, StoreImage and MoveImage.
 * The other SYS.OBJ functions are in neighbouring units because their
 * reconstructions need different compiler options or conflicting
 * declarations.
 */
/* SYS stays in 18 units: no option set reproduces more than a few
 * neighbours. sys.c, SetDrawEnv, Gpu_DmaVramTransfer, vram_transfer,
 * dma_queue and LoadImage2 only match with GCC 2.8.1 and
 * -mno-split-addresses, PutDispEnv, Gpu_BuildDrawModeCmd, Gpu_SubmitPacket,
 * Gpu_ResetDmaWaitTimer and Gpu_RestoreDmaCallback only with GCC 2.7.2 and
 * drawarea only with -O1. */
#include "pe1/psyq_callbacks.h"
#include "pe1/gpu_callbacks.h"
#include "common.h"
#include "pe1/gpu_state.h"
#include "include_asm.h"
#include "pe1/psyq_gpu.h"

extern unsigned char D_8009574E;
extern unsigned char D_80095704[];
extern char D_800117E0[], D_80011800[];
extern unsigned short D_800957CC[][2], D_800957D8[][2];
extern GpuCallbacks *D_80095744;
extern GpuDebugPrintf D_80095748;
int printf(char *, ...);
void GPU_memset(void *, int, int);
void GPU_cw(unsigned int);
int Gpu_InitDmaQueue(int);

int ResetGraph(int mode) {
    GpuSystemState *state;
    void *drawCache;
    volatile unsigned char *gpuType;
    switch (mode & 7) {
    case 0:
    case 3:
        {
            char *format;
            unsigned char *version;

            format = D_800117E0;
            version = D_80095704;
            printf(format, version, (GpuSystemState *)&D_8009574C);
        }
    case 5:
        state = (GpuSystemState *)&D_8009574C;
        gpuType = &state->status.type;
        GPU_memset(state, 0, sizeof(*state));
        ResetCallback();
        GPU_cw((unsigned int)D_80095744 & 0xFFFFFF);
        *gpuType = Gpu_InitDmaQueue(mode);
        drawCache = &state->drawCache;
        state->status.queueState.queue = 1;
        state->status.width = D_800957CC[*gpuType][0];
        state->status.height = D_800957D8[*gpuType][0];
        GPU_memset(drawCache, -1, sizeof(state->drawCache));
        GPU_memset(&state->displayCache, -1, sizeof(state->displayCache));
        return *gpuType;
    default:
        if (D_8009574E >= 2) D_80095748(D_80011800, mode);
        return D_80095744->reset(1);
    }
}

extern char D_80011814[];

int SetGraphDebug(int debugLevel) {
    u8 *currentDebugLevel;
    int result;
    GpuDebugPrintf debugPrint;
    int currentLevel;
    int type;
    int reverse;
    int oldDebugLevel;

    currentDebugLevel = &D_8009574C.queueState.debugLevel;
    /* Preserve the shared base used for the adjacent GPU state bytes. */
        oldDebugLevel = *currentDebugLevel;
    *currentDebugLevel = debugLevel;
    result = oldDebugLevel;

    if ((u8)debugLevel == 0) {
        return result;
    }

    debugPrint = D_80095748;
    /* Keep the callback load ahead of its arguments. */
        currentLevel = currentDebugLevel[0];
    type = currentDebugLevel[-2];
    reverse = currentDebugLevel[1];
    /* Materialize the byte arguments before the format string address. */
        debugPrint(D_80011814, currentLevel, type, reverse);
    result = oldDebugLevel;
    return result;
}

int SetGraphQueue(int mode)
{
    GpuQueueState *state = &D_8009574C.queueState;
    int previous = state->queue;
    if (state->debugLevel >= 2) {
        /* Keep the two prior-state paths: stock GCC merges the calls after
         * choosing the retail register lifetimes. Both log exactly once. */
        if (previous) {
            D_80095748(D_80011840, mode);
        } else {
            D_80095748(D_80011840, mode);
        }
    }
    if (mode != state->queue) {
        D_80095744->reset(1);
        state->queue = mode;
        DMACallback(2, 0);
    }
    return previous;
}

int GetGraphDebug(void) {
    return D_8009574C.queueState.debugLevel;
}

extern char D_80011854[];

void *DrawSyncCallback(void *callback) {
    void *previous;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011854, callback);
    }

    previous = (void *)D_8009574C.drawSyncCallback;
    D_8009574C.drawSyncCallback = (void (*)())callback;
    return previous;
}

extern char D_80011870[];
extern GpuCallbacks *g_GpuCallbacks[];
extern GpuDebugPrintf g_GpuDebugPrintf[];
extern u8 g_GraphDebug[];

void GPU_memset(void *destination, int value, int size);

void SetDispMask(int mask) {
    u8 *debug = g_GraphDebug;
    u8 *clearPointer;
    GpuCallbacks *callbacks;
    int command;

    if (*debug >= 2) {
        g_GpuDebugPrintf[0](D_80011870, mask);
    }

    clearPointer = debug + 0x6A;
    if (mask == 0) {
        GPU_memset(clearPointer, -1, 20);
    }

    command = 0x03000000;
    asm("" : "+r"(command));
    callbacks = g_GpuCallbacks[0];
    if (mask == 0) {
        command |= 1;
    } else {
        command = 0x03000000;
    }
    callbacks->callback10(command);
}

extern char D_80011884[];

int DrawSync(int arg0) {
    void (*fn)(char *, int);
    GpuCallbacks *callbacks;

    if (D_8009574C.queueState.debugLevel >= 2) {
        fn = D_80095748;
        fn(D_80011884, arg0);
    }

    callbacks = D_80095744;
    return callbacks->callback3C(arg0);
}

extern char D_800118B8[], D_800118A4[], D_80011898[];

void checkRECT(char *name, RECT *rect) {
    char *format;
    switch (D_8009574C.queueState.debugLevel) {
    case 1:
        if (rect->w > D_8009574C.width || rect->w + rect->x > D_8009574C.width ||
            rect->y > D_8009574C.height || rect->y + rect->h > D_8009574C.height ||
            rect->w <= 0 || rect->x < 0 || rect->y < 0 || rect->h <= 0) {
            format = D_80011898;
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

extern char D_800118BC[];
extern char D_800118C8[];

void checkRECT(char *message, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118BC, rect);
    color = (b << 16) | (g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

int ClearImage2(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118C8, rect);
    color = 0x80000000 | (b << 16) | (g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

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

        destinationPosition = y << 16;
        packetData = D_800957EC;
        asm("" : "+r"(packetData));
        destinationX = x & 0xFFFF;
        destinationPosition |= destinationX;
        sourcePosition = ((GpuRectWords *)rect)->position;
        callbacks = D_80095744;
        asm("" : "+r"(sourcePosition), "+r"(callbacks));
        packetSize = 20;
        asm("" : "+r"(packetSize));
        packetData[1] = destinationPosition;
        *packetData = sourcePosition;
        size = ((GpuRectWords *)rect)->size;
        packet = packetData - 2;
        packetData[2] = size;
        asm("" : : "r"(size) : "memory");
        return callbacks->addque2(callbacks->u18.moveImage, packet, packetSize,
                                  0);
    }
}
