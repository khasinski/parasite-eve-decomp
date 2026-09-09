#include "common.h"

#include "pe1/memcard_state.h"

int Spu_CheckTimerElapsed(void);
int MemCard_WaitReadyForTransfer(void)
{
  register MemCardInterruptRegisters *state;
  MemCardSioRegisters *regs;
  MemCardSioRegisters *check_regs;
  int value;
  unsigned char status;
  state = g_MemCardState;
  regs = g_MemCardSioRegs;
  value = -0x81;
  state->status = value;
  status = regs->status;
  status &= 0x80;
  while (status != 0)
  {
    if (Spu_CheckTimerElapsed() != 0)
    {
      return 0;
    }
    check_regs = g_MemCardSioRegs;
    status = check_regs->status;
    status &= 0x80;
  }
  regs = g_MemCardSioRegs;

  regs->control |= 0x10;
  return 1;
}

void MemCard_WaitStatusBit2(void) {
    MemCardSioRegisters *ptr = g_MemCardSioRegs;

    /* Keep the poll loop target on the lhu, not the load-delay nop. */
    asm volatile("nop");
    while ((ptr->status & 2) == 0) {
    }
}
