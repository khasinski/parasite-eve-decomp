/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBETC INTR, part 2 of 2: Sys_InitIntrManager, Render_InitSceneGeom,
 * Sys_SetIntrCallback, Sys_SaveDisableIntr, Sys_RestoreIntr and
 * Sys_ClearWordBuf. */

#include "common.h"
#include "pe1/psyq_api_internal.h"
#include "pe1/psyq_bios.h"

#include "intr_2_externs.h"

void Sys_ClearWordBuf(int *dst, int count);
int setjmp(void *env);
void _96_remove(void *arg);

u16 *Sys_InitIntrManager(void) {
    register InterruptCallbackState *intr asm("$16");
    u32 *entrySp;
    u16 *dst;
    volatile u16 *src;
    InterruptDispatchTable *ptr;
    InterruptDispatchTable *vsyncPtr;
    VSyncCallbackSetter result;
    DmaCallbackSetter dmaSetter;

    intr = (InterruptCallbackState *)g_IntrState;
    if (intr->active != 0) {
        return 0;
    }

    dst = g_IntrStatusRegPtr;
    src = D_8009513C;
    *src = 0;
    *dst = *src;
    *g_IntrDmaControlRegPtr = 0x33333333;

    Sys_ClearWordBuf((int *)intr, sizeof(InterruptCallbackState) / 4);
    if (setjmp(intr->entry) != 0) {
        Render_InitSceneGeom();
    }

    /* The interrupt context's stack pointer: near the top of its stack. */
    entrySp = &((InterruptCallbackState *)g_IntrState)->entry[1];
    *entrySp = PSX_ADDRESS(&((InterruptCallbackState *)g_IntrState)->interruptStack[1004]);
    HookEntryInt(((InterruptCallbackState *)g_IntrState)->entry);
    ((InterruptCallbackState *)g_IntrState)->active = 1;

    result = startIntrVSync();
    vsyncPtr = g_EventCallbackTable;
    vsyncPtr->vsync = result;

    dmaSetter = startIntrDMA();
    ptr = g_EventCallbackTable;
    ptr->dma = dmaSetter;

    _96_remove(g_EventCallbackTable);
    intr = (InterruptCallbackState *)g_IntrState;
    ExitCriticalSection();

    return &intr->active;
}

/* Contiguous trapIntr, setIntr, stopIntr and restartIntr from INTR.OBJ. */
/* Legacy project name: this is LIBETC trapIntr, not scene setup. */
void Render_InitSceneGeom(void)
{
    InterruptCallbackState *state = (InterruptCallbackState *) g_IntrState;
    unsigned short pending;
    unsigned short bits;
    int channel;
    PsyqInterruptHandler *handlers;
    if (!state->active)
    {
        printf(g_IntrUnexpectedFormat, *g_IntrStatusRegPtr);
        ReturnFromException();
    }
    state->inCallback = 1;
    bits = (state->enabled & (*g_IntrStatusRegPtr)) & (*((u16 *) D_8009513C));
    pending = bits;
    if (bits)
    {
        int bit = 1;
        handlers = state->handlers;
        do
        {
            for (channel = 0; pending && (channel < 11); channel++, pending >>= 1)
            {
                if (pending & 1)
                {
                    *g_IntrStatusRegPtr = ~(bit << channel);
                    if (handlers[channel])
                    {
                        handlers[channel]();
                    }
                }
            }

            bits = (g_IntrEnabledMask & (*g_IntrStatusRegPtr)) & (*((u16 *) D_8009513C));
            pending = bits;
        }
        while (bits);
    }

    if ((*g_IntrStatusRegPtr) & (*((u16 *) D_8009513C)))
    {
        if ((g_IntrTimeoutCounter++) > 0x800)
        {
            const char *format = g_IntrTimeoutFormat;
            /* These equivalent calls let GCC merge the branch after scheduling
              * the format address before the two MMIO reads, as in the SDK. */
            if (g_IntrStatusRegPtr) {
                printf(format, *g_IntrStatusRegPtr, *((u16 *)D_8009513C));
            } else {
                printf(g_IntrTimeoutFormat, *g_IntrStatusRegPtr, *((u16 *)D_8009513C));
            }
            g_IntrTimeoutCounter = 0;
            *g_IntrStatusRegPtr = 0;
        }
    }
    else
    {
        g_IntrTimeoutCounter = 0;
    }

    g_IntrInCallback = 0;
    ReturnFromException();
}

PsyqInterruptHandler Sys_SetIntrCallback(int channel, PsyqInterruptHandler handler) {
    PsyqInterruptHandler *handlers = g_IntrHandlers;
    InterruptCallbackState *state;
    PsyqInterruptHandler old = handlers[channel];
    unsigned int mask;
    if (handler == old) {
        return old;
    }
    state = (InterruptCallbackState *)((char *)handlers -
                      PE1_OFFSETOF(InterruptCallbackState, handlers));

    if (state->active) {
        mask = *D_8009513C;
        *D_8009513C = 0;
        if (handler) {
            handlers[channel] = handler;
            mask |= 1 << channel;
            state->enabled |= 1 << channel;
        } else {
            handlers[channel] = 0;
            mask &= ~(1 << channel);
            ((InterruptCallbackState *)((char *)handlers -
                      PE1_OFFSETOF(InterruptCallbackState, handlers)))->enabled &= ~(1 << channel);
        }
        if (channel == 0) {
            int clear = !handler;
            ChangeClearPAD(clear);
            ChangeClearRCnt(3, clear);
        }
        /* This loop scope preserves the SDK register allocation under GCC 2.8.1. */
        do {
            if (channel == 4) ChangeClearRCnt(0, !handler);
            if (channel == 5) ChangeClearRCnt(1, !handler);
            if (channel == 6) ChangeClearRCnt(2, !handler);
        } while (0);
        *D_8009513C = mask;
    }
    return old;
}

InterruptCallbackState *Sys_SaveDisableIntr(void) {
    InterruptCallbackState *state;
    /* Keep the I_MASK pointer in the register used by the retail stopIntr. */
    register volatile u16 *mask asm("$2");
    u32 *control;
    u16 *status;
    u32 value;

    state = (InterruptCallbackState *)g_IntrState;
    if (state->active == 0) {
        return 0;
    }

    EnterCriticalSection();
    mask = g_IntrMaskRegPtr;
    control = g_IntrDmaControlRegPtr;
    state->interruptMask = *mask;
    value = *control;
    status = g_IntrStatusRegPtr;
    state->dmaControl = value;
    *mask = 0;
    *status = *mask;

    control = g_IntrDmaControlRegPtr;
    *control &= 0x77777777;
    ResetEntryInt();
    state->active = 0;
    return state;
}

InterruptCallbackState *Sys_RestoreIntr(void) {
    InterruptCallbackState *state = (InterruptCallbackState *)g_IntrState;
    InterruptCallbackState *result;
    if (!state->active) {
        HookEntryInt(state->entry);
        state->active = 1;
        *D_8009513C = state->interruptMask;
        *(volatile u32 *)g_IntrDmaControlRegPtr = state->dmaControl;
        ExitCriticalSection();
        result = state;
    } else {
        result = 0;
    }
    return result;
}

void Sys_ClearWordBuf(int *dst, int count) {
    int i = count - 1;

    if (count != 0) {
        do {
            *dst = 0;
            i--;
            dst++;
        } while (i != -1);
    }
}
