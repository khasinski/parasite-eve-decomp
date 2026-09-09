#include "common.h"

#include "pe1/memcard_state.h"

typedef struct 
{
  volatile unsigned char field0;
  unsigned char pad1[3];
  volatile u16 field4;
  u16 pad6;
  volatile u16 field8;
  volatile u16 fieldA;
  u16 padC;
  volatile u16 fieldE;
} MemCardSioRegs;
extern MemCardSioRegs * volatile D_8009B788;
int Spu_CheckTimerElapsed(void);
int MemCard_WaitReadyForTransfer(void)
{
  register MemCardInterruptRegisters *state;
  MemCardSioRegs *regs;
  MemCardSioRegs *check_regs;
  int value;
  unsigned char status;
  state = g_MemCardState;
  regs = D_8009B788;
  value = -0x81;
  state->status = value;
  status = regs->field4;
  status &= 0x80;
  if (status == 0)
  {
    goto ready;
  }
  loop:
  if (Spu_CheckTimerElapsed() != 0)
  {
    return 0;
  }

  check_regs = D_8009B788;
  status = check_regs->field4;
  status &= 0x80;
  if (status != 0)
  {
    goto loop;
  }
  ready:
  regs = D_8009B788;

  regs->fieldA |= 0x10;
  return 1;
}
extern volatile unsigned short *g_MemCardSioRegs;

void MemCard_WaitStatusBit2(void) {
    volatile unsigned short *ptr = g_MemCardSioRegs;

    /* Keep the poll loop target on the lhu, not the load-delay nop. */
    asm volatile("nop");
    while ((ptr[2] & 2) == 0) {
    }
}
