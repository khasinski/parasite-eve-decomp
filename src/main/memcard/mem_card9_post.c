/* MASPSX_FLAGS: --store-jump-delay */

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
extern int D_800A5AC4;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    volatile unsigned char field0;
    unsigned char pad1[3];
    volatile u16 field4;
    u16 pad6;
    u16 pad8;
    volatile u16 fieldA;
} TimerRegs;

extern TimerRegs * volatile g_MemCardSioRegs;
extern TimerRegs *D_8009B788;
extern volatile int *D_8009B784;
extern int D_800A76D0;
extern int D_800BD02C;

int Spu_CheckTimerElapsed(void);
int MemCard_DmaProcess();
void MemCard_RunCommandStep();

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9_post", MemCard_DmaProcess);

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

typedef struct {
    char pad0[0x3C];
    u8 *buffer;
    u8 *out;
    u8 index;
    u8 count;
} MemCardTransfer;

int MemCard_ReadByte(MemCardTransfer *transfer, int value) {
    register TimerRegs *regs asm("$2");
    register int data asm("$18") = value;
    register int result asm("$17");
    register int control asm("$6");
    register u8 *out asm("$3");
    volatile unsigned short *timer_count;

    if (data < 0) {
        regs = D_8009B788;
        out = transfer->out;
        result = regs->field0;
        transfer->index = 0xFF;
        transfer->count = 1;
        *out = ~data;
        regs = D_8009B788;
        while ((regs->field4 & 1) == 0) {
        }
        while (Spu_CheckTimerElapsed() == 0) {
        }
        D_8009B788->field0 = ~data;
        return result;
    }

    if ((transfer->buffer[0] >> 4) == 8) {
        control = 0x88;
        if (transfer->index >= 9) {
            control = 0x22;
        }
    } else {
        control = 0x88;
    }

    timer_count = (volatile unsigned short *)0x1F801120;
    regs = D_8009B788;
    D_800BD02C = 0x1AE;
    D_800A76D0 = *timer_count;
    while ((regs->field4 & 2) == 0) {
    }

    regs = D_8009B788;
    result = regs->field0;
    regs->fieldA = control;
    while ((*D_8009B784 & 0x80) == 0) {
        if (Spu_CheckTimerElapsed() != 0) {
            return -0x14;
        }
    }

    D_8009B788->field0 = data;
    transfer->count++;
    transfer->buffer[transfer->index] = result;
    transfer->index++;
    return result;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9_post", MemCard_WriteByte);

int MemCard_WaitReadyForTransfer(void) {
    register MemCardState *state asm("$3");
    TimerRegs *regs;
    TimerRegs *check_regs;
    register int value asm("$2");
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
