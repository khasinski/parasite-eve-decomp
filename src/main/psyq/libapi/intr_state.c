/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "pe1/psyq_api_internal.h"
#include "pe1/psyq_bios.h"

/* Contiguous setIntr, stopIntr and restartIntr fragment of LIBETC INTR.OBJ. */
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
