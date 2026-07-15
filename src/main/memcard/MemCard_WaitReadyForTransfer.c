/* MASPSX_FLAGS: --store-jump-delay */

typedef unsigned short u16;

typedef struct {
    unsigned int field0;
    unsigned int field4;
} MemCardState;

typedef struct {
    volatile unsigned char field0;
    unsigned char pad1[3];
    volatile u16 field4;
    u16 pad6;
    volatile u16 field8;
    volatile u16 fieldA;
    u16 padC;
    volatile u16 fieldE;
} MemCardSioRegs;

extern MemCardState * volatile D_8009B784;
extern MemCardSioRegs * volatile D_8009B788;

int Spu_CheckTimerElapsed(void);

int MemCard_WaitReadyForTransfer(void) {
    register MemCardState *state asm("$3");
    MemCardSioRegs *regs;
    MemCardSioRegs *check_regs;
    register int value asm("$2");
    int status;

    state = D_8009B784;
    regs = D_8009B788;
    value = -0x81;
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
    check_regs = D_8009B788;
    status = check_regs->field4;
    status &= 0x80;
    if (status != 0) {
        goto loop;
    }

ready:
    regs = D_8009B788;
    regs->fieldA |= 0x10;
    return 1;
}
