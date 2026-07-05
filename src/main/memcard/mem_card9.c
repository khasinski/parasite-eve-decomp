/* MASPSX_FLAGS: --store-return-delay */
#include "include_asm.h"

#include "include_asm.h"

#include "pe1/memcard_state.h"

extern int (*D_8009B74C)(void);

extern int g_MemCardCallbackPending;

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

int MemCard_TimerReadyCallback(void) {
    MemCardState *state = g_MemCardState;

    if ((state->field4 & 1) == 0) {
        return 0;
    }
    if ((state->field0 & 1) == 0) {
        return 0;
    }
    if (D_8009B74C != 0) {
        D_8009B74C();
    }
    return 1;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_TimerCallback);

int MemCard_TakeCallback(void) {
    int old = g_MemCardCallbackPending;
    g_MemCardCallbackPending = 0;
    return old;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_StartCounterIrq);

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_DmaProcess);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/mem_card9", MemCard_RunCommandStep);

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
