/* MASPSX_FLAGS: --store-return-delay */
#include "include_asm.h"

#include "include_asm.h"

#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);
typedef int (*MemCardStepFn)(void);
typedef void (*MemCardErrorFn)(int);

extern int g_MemCardCallbackPending;
extern int D_8009B75C;
extern int D_8009B758;
extern int D_8009B764;
extern int D_8009B768;
extern int D_8009B76C;
extern int D_8009B774;
extern int D_8009B778;
extern int D_8009B78C;
extern void *D_8009B788;
extern int D_800A5AC4;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

typedef unsigned short u16;

typedef struct {
    unsigned char field0;
    unsigned char pad1[3];
    u16 field4;
    u16 pad6;
    u16 pad8;
    u16 fieldA;
} TimerRegs;

extern TimerRegs * volatile g_MemCardSioRegs;

int Spu_CheckTimerElapsed(void);
int MemCard_DmaProcess();
void MemCard_RunCommandStep();

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_DmaProcess);

void Timer_StartTimeout(int frames);
int MemCard_WaitReadyForTransfer(void);

void MemCard_RunCommandStep(void) {
    register int *state asm("$5");
    int result;
    register MemCardStepFn fn asm("$2");
    register int step asm("$3");
    register int next asm("$2");

    state = &D_8009B768;
    step = *state;
    fn = D_8009B7A8[step];
    *state = step + 1;
    result = fn();
    if (result >= 0) {
        if (D_8009B768 != 0) {
            Timer_StartTimeout(0x3C);
            if (MemCard_WaitReadyForTransfer() == 0) {
                D_8009B724(-3);
            }
        }
        step = D_8009B768;
        if (step >= 5) {
            next = step - 1;
            D_8009B768 = next;
        }
    } else {
        D_8009B724(result);
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_ReadByte);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_WriteByte);

int MemCard_WaitReadyForTransfer(void) {
    MemCardState *state;
    TimerRegs *regs;
    TimerRegs *check_regs;
    int value;
    int status;

    value = -0x81;
    state = g_MemCardState;
    regs = g_MemCardSioRegs;
    state->field0 = value;
    status = regs->field4;
    status &= 0x80;
    if (status == 0) {
        goto ready;
    }

loop:
    if (Spu_CheckTimerElapsed() != 0) {
        return 0;
    }
    check_regs = g_MemCardSioRegs;
    status = check_regs->field4;
    status &= 0x80;
    if (status != 0) {
        goto loop;
    }

ready:
    regs = g_MemCardSioRegs;
    regs->fieldA |= 0x10;
    return 1;
}
