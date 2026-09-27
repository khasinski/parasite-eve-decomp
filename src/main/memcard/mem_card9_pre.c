#include "common.h"
#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);
typedef int (*MemCardStepFn)(void);
typedef void (*MemCardErrorFn)(int);

extern int g_MemCardCallbackPending;
/* Matching debt: AT holds the upper address of the pending flag so its
 * store can occupy a branch delay slot. This is an address bias, not a
 * separate memory-card data structure. Each user initializes it before use. */
register int *g_MemCardPendingAddressBase asm("$1");
extern int D_8009B75C;
extern int D_8009B758;
extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B76C;
extern int D_8009B774;
extern int D_8009B778;
extern int D_8009B78C;
extern int D_800A5AC0;
extern int D_800A5AC4;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

int Spu_CheckTimerElapsed(void);
int _padInitSioMode();
void MemCard_RunCommandStep();

int MemCard_TimerReadyCallback(void) {
    MemCardInterruptRegisters *state = g_MemCardState;

    if ((state->mask & 1) == 0) {
        return 0;
    }
    if ((state->status & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}

int MemCard_TimerCallback(void) {
    int index;
    int limit;
    int *timer;
    void *obj;
    register int active asm("$3");
    register int one asm("$2");

    active = D_8009B774;
    one = 1;
    asm volatile("" : : "r"(active), "r"(one) : "memory");
    g_MemCardPendingAddressBase = (int *)0x800A0000;
    g_MemCardPendingAddressBase[-0x121D] = one;
    if (active != 0) {
        timer = &D_800A5AC0;
        active = *timer;
        if (active < 0x96) {
            one = active + 1;
            *timer = one;
        }
    }

    if (D_8009B778 == 0) {
        timer = &D_800A5AC4;
        active = *timer;
        if (active < 0x96) {
            one = active + 1;
            *timer = one;
        }
    }

    if (D_8009B75C != 0) {
        index = D_8009B774;
        limit = D_8009B778;
        if (limit >= index) {
            obj = (void *)((u32)D_8009B758 + ((((u32)index << 4) - (u32)index) << 4));
            D_8009B768 = 0;
            D_8009B764 = index;
            if (_padInitSioMode(obj) == 0) {
                D_8009B724(0xFFFF);
            }

            D_8009B76C = 0;
            while (D_8009B778 >= D_8009B764) {
                index = D_8009B764;
                obj = (void *)((u32)D_8009B758 + ((((u32)index << 4) - (u32)index) << 4));
                MemCard_RunCommandStep(obj);
            }
            g_MemCardSioRegs->baud = 0x88;
        }
    }

    return 0;
}

int MemCard_TakeCallback(void) {
    int old = g_MemCardCallbackPending;
    g_MemCardPendingAddressBase = (int *)0x800A0000;
    g_MemCardPendingAddressBase[-0x121D] = 0;
    return old;
}
