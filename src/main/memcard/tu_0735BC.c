#include "include_asm.h"

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void ChangeClearRCnt(int, int);
extern void SysDeqIntRP(int, void *);
extern int g_MemCardCounterIrqQueueNode;

typedef unsigned short u16;

#include "pe1/memcard_state.h"

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

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_0735BC", MemCard_DmaProcess);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_0735BC", MemCard_RunCommandStep);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_0735BC", MemCard_ReadByte);

INCLUDE_ASM("asm/USA/main/nonmatchings/memcard/tu_0735BC", MemCard_WriteByte);

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
