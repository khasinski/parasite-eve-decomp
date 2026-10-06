/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBETC INTR, part 2 of 3: Sys_InitIntrManager, Render_InitSceneGeom,
 * Sys_SetIntrCallback, Sys_SaveDisableIntr, Sys_RestoreIntr. */
#include "common.h"
#include "pe1/psyq_api_internal.h"
#include "pe1/psyq_bios.h"

extern u16 D_800945E4[];
extern u16 D_80094620[];
extern u16 *D_80095670;
extern volatile u16 *D_80095674;
extern u32 *D_80095678;

void Sys_ClearWordBuf(void *dst, int count);
int setjmp(void *env);
void Render_InitSceneGeom(void);
void HookEntryInt(void *entry);
void _96_remove(void *arg);
void ExitCriticalSection(void);

u16 *Sys_InitIntrManager(void) {
    register u16 *state asm("$16");
    u16 *dst;
    volatile u16 *src;
    u16 *flag;
    InterruptDispatchTable *ptr;
    InterruptDispatchTable *vsyncPtr;
    VSyncCallbackSetter result;
    DmaCallbackSetter dmaSetter;

    state = D_800945E4;
    if (state[0] != 0) {
        return 0;
    }

    dst = D_80095670;
    src = D_80095674;
    *src = 0;
    *dst = *src;
    *D_80095678 = 0x33333333;

    Sys_ClearWordBuf(state, 0x41A);
    if (setjmp(state + 0x1C) != 0) {
        Render_InitSceneGeom();
    }

    state = D_80094620;
    *((u32 *)state) = (u32)(state + 0x7EE);
    HookEntryInt(state - 2);
    flag = state - 0x1E;
    *flag = 1;

    result = startIntrVSync();
    vsyncPtr = g_EventCallbackTable;
    vsyncPtr->vsync = result;

    dmaSetter = startIntrDMA();
    ptr = g_EventCallbackTable;
    ptr->dma = dmaSetter;

    _96_remove(g_EventCallbackTable);
    state = flag;
    ExitCriticalSection();

    return state;
}

/* Contiguous trapIntr, setIntr, stopIntr and restartIntr from INTR.OBJ. */
/* Legacy project name: this is LIBETC trapIntr, not scene setup. */
void Render_InitSceneGeom(void)
{
    InterruptCallbackState *state = (InterruptCallbackState *) D_800945E4;
    unsigned short pending;
    unsigned short bits;
    int channel;
    PsyqInterruptHandler *handlers;
    if (!state->active)
    {
        printf(D_80011740, *D_80095670);
        ReturnFromException();
    }
    state->inCallback = 1;
    bits = (state->enabled & (*D_80095670)) & (*((u16 *) D_80095674));
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
                    *D_80095670 = ~(bit << channel);
                    if (handlers[channel])
                    {
                        handlers[channel]();
                    }
                }
            }

            bits = (D_80094614 & (*D_80095670)) & (*((u16 *) D_80095674));
            pending = bits;
        }
        while (bits);
    }

    if ((*D_80095670) & (*((u16 *) D_80095674)))
    {
        if ((D_8009567C++) > 0x800)
        {
            const char *format = D_8001175C;
            /* These equivalent calls let GCC merge the branch after scheduling
              * the format address before the two MMIO reads, as in the SDK. */
            if (D_80095670) {
                printf(format, *D_80095670, *((u16 *)D_80095674));
            } else {
                printf(D_8001175C, *D_80095670, *((u16 *)D_80095674));
            }
            D_8009567C = 0;
            *D_80095670 = 0;
        }
    }
    else
    {
        D_8009567C = 0;
    }

    D_800945E6 = 0;
    ReturnFromException();
}


PsyqInterruptHandler Sys_SetIntrCallback(int channel, PsyqInterruptHandler handler) {
    PsyqInterruptHandler *handlers = D_800945E8;
    InterruptCallbackState *state;
    PsyqInterruptHandler old = handlers[channel];
    unsigned int mask;
    if (handler == old) {
        return old;
    }
    state = (InterruptCallbackState *)((char *)handlers -
                      PE1_OFFSETOF(InterruptCallbackState, handlers));

    if (state->active) {
        mask = *D_80095674;
        *D_80095674 = 0;
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
        *D_80095674 = mask;
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

    state = (InterruptCallbackState *)D_800945E4;
    if (state->active == 0) {
        return 0;
    }

    EnterCriticalSection();
    mask = g_IntrMaskRegPtr;
    control = D_80095678;
    state->interruptMask = *mask;
    value = *control;
    status = D_80095670;
    state->dmaControl = value;
    *mask = 0;
    *status = *mask;

    control = D_80095678;
    *control &= 0x77777777;
    ResetEntryInt();
    state->active = 0;
    return state;
}

InterruptCallbackState *Sys_RestoreIntr(void) {
    InterruptCallbackState *state = (InterruptCallbackState *)D_800945E4;
    InterruptCallbackState *result;
    if (!state->active) {
        HookEntryInt(state->entry);
        state->active = 1;
        *D_80095674 = state->interruptMask;
        *(volatile u32 *)D_80095678 = state->dmaControl;
        ExitCriticalSection();
        result = state;
    } else {
        result = 0;
    }
    return result;
}
