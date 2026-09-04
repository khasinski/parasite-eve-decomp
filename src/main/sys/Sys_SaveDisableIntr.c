/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */

#include "common.h"

typedef struct InterruptCallbackState {
    u16 active;
    u8 pad02[0x30];
    u16 interruptMask;
    u32 dmaControl;
} InterruptCallbackState;

extern InterruptCallbackState D_800945E4;
extern u16 *D_80095670;
extern volatile u16 *g_IntrMaskRegPtr;
extern u32 *D_80095678;

int EnterCriticalSection(void);
int ResetEntryInt(void);

InterruptCallbackState *Sys_SaveDisableIntr(void) {
    InterruptCallbackState *state;
    register volatile u16 *mask asm("$2");
    u32 *control;
    u16 *status;
    u32 value;

    state = &D_800945E4;
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
