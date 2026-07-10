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
extern int D_8009B77C[];
extern int D_8009B78C;
extern int D_800A5AC4;
extern MemCardStepFn D_8009B7A8[];
extern MemCardErrorFn D_8009B724;
extern void (*D_8009B744)(void *);
extern void (*D_8009B748)(void *);

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
    volatile u16 pad8;
    volatile u16 fieldA;
    u16 padC;
    volatile u16 fieldE;
} TimerRegs;

typedef struct {
    u8 pad0[0x0C];
    int fd;
    u8 pad10[0x26];
    u8 field_36;
    u8 pad37[5];
    u8 *buffer;
    u8 *out;
    u8 index;
    u8 count;
    u8 pad46[0x0A];
    u8 pad50;
    u8 pad51[0x97];
    u8 field_e8;
} MemCardTransfer;

extern TimerRegs * volatile g_MemCardSioRegs;
extern TimerRegs *D_8009B788;
extern volatile int *D_8009B784;
extern int D_800A76D0;
extern int D_800BD02C;

int Spu_CheckTimerElapsed(void);
int MemCard_DmaProcess();
void MemCard_RunCommandStep();
void Timer_StartTimeout(int frames);
int MemCard_WaitStatusBit2(void);

void MemCard_StopCounterIrq(void) {
    EnterCriticalSection();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, &g_MemCardCounterIrqQueueNode);
    ExitCriticalSection();
}

int MemCard_DmaProcess(MemCardTransfer *transfer) {
    register MemCardTransfer *transfer_reg asm("$16");
    register int *counts asm("$17");
    TimerRegs *regs;
    register TimerRegs *regs_after_timeout asm("$4");
    int index;
    int count;
    register int mode asm("$3");

    transfer_reg = transfer;
    regs = D_8009B788;
    regs->fieldA = 0x40;
    regs->fieldA = 0;
    regs->pad8 = 0xD;
    regs->fieldE = 0x88;

    if (transfer_reg->field_e8 == 8) {
        Timer_StartTimeout(0x50);
    } else {
        Timer_StartTimeout(0x91);
    }

    index = D_8009B764;
    regs_after_timeout = D_8009B788;
    mode = 0x1003;
    if (index != 0) {
        mode = 0x3003;
    }
    regs_after_timeout->fieldA = mode;

    count = D_8009B77C[index];
    if (count >= 0) {
        counts = D_8009B77C;
        if (count > 0) {
            do {
                index = D_8009B764;
                counts[index]--;
                D_8009B744((void *)(transfer_reg->fd + ((counts[index] << 4) - counts[index]) * 0x10));
            } while (counts[D_8009B764] > 0);
        }

        index = D_8009B764;
        if (counts[index] == 0) {
            counts[index] = -1;
            D_8009B744(transfer_reg);
            D_8009B748(transfer_reg);
        }
    }

    regs = D_8009B788;
    if ((regs->field4 & 0x200) != 0) {
        regs->fieldA |= 0x10;
        if ((regs->field4 & 0x200) != 0) {
            regs->field0 = 1;
            MemCard_WaitStatusBit2();
            regs = D_8009B788;
            (void)regs->field0;
            return 0;
        }
        *D_8009B784 = -0x81;
    }

    if (transfer_reg->pad50 != 0 && transfer_reg->field_36 != 0) {
        return 0;
    }

    transfer_reg->buffer[0] = 0;
    return 1;
}

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

int MemCard_WriteByte(MemCardTransfer *transfer, int value) {
    register MemCardTransfer *transfer_reg asm("$16");
    register int data asm("$19") = value;
    register int control asm("$17");
    register int result asm("$18");
    TimerRegs *regs;
    volatile int *status;
    volatile u16 *timer_count;
    volatile u16 *timer_target;
    volatile u16 *timer_mode;
    unsigned int now;
    unsigned int base;
    unsigned int limit;
    unsigned int elapsed;

    transfer_reg = transfer;

    control = 0x88;
    if ((transfer_reg->buffer[0] >> 4) == 8) {
        if (transfer_reg->index >= 9) {
            control = 0x22;
        }
    }

    regs = D_8009B788;
    while ((regs->field4 & 2) == 0) {
    }

    Timer_StartTimeout(0x190);

    regs = D_8009B788;
    result = regs->field0;
    if (transfer_reg->index == 0 && (result >> 4) == 8) {
        regs->fieldE = 0x22;
    } else {
        regs->fieldE = control;
    }

    status = D_8009B784;
    if ((*status & 0x80) == 0) {
        timer_count = (volatile u16 *)0x1F801120;
        timer_target = (volatile u16 *)0x1F801128;
        timer_mode = (volatile u16 *)0x1F801124;
        base = D_800A76D0;
        limit = D_800BD02C;
        do {
            now = *timer_count;
            if (now < base) {
                if (*timer_target != 0) {
                    now += *timer_target;
                } else {
                    now += 0x10000;
                }
            }

            elapsed = now - base;
            if ((*timer_mode & 0x200) != 0) {
                if (limit <= elapsed) {
                    return -2;
                }
            } else if (limit <= (elapsed >> 3)) {
                return -2;
            }
        } while ((*status & 0x80) == 0);
    }

    if (transfer_reg->field_e8 != 8 && D_8009B768 == 2) {
        Timer_StartTimeout(0x3C);
        while (Spu_CheckTimerElapsed() == 0) {
        }
    }

    D_8009B788->field0 = data;
    transfer_reg->count++;
    if (transfer_reg->index != 0xFF) {
        transfer_reg->buffer[transfer_reg->index] = result;
    }
    transfer_reg->index++;
    return result;
}

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
