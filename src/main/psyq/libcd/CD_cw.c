/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fno-expensive-optimizations -mno-split-addresses */
#include "pe1/psyq_cd.h"

#include "pe1/psyq_bios.h"

/* Compare the previous poll count, then commit the incremented counter. */
static inline int count_poll(void) {
    int count = D_800A347C;
    int old = count++;
    D_800A347C = count;
    return old;
}

static inline int timed_out(char **commands, char **events, u8 *sync) {
    if (VSync(-1) > D_800A3478 || count_poll() > 0x3C0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[D_8009AFD5],
               events[((CdInterruptEvents *)sync)->sync],
               events[((CdInterruptEvents *)sync)->ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

static inline void copy_result(u8 *destination, const volatile u8 *source) {
    int remaining;
    if (destination) {
        remaining = 7;
        for (; remaining != -1; remaining--) {
            *destination++ = *source++;
        }
    }
}

static inline void dispatch_interrupts(u8 *sync, u8 *ready) {
    int bank = *D_8009B27C & 3;
    int pending;
    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8)
            D_8009AFB8(*ready, D_800A3468);
        if ((pending & 2) && D_8009AFB4)
            D_8009AFB4(*sync, D_800A3460);
    }
    *D_8009B27C = bank;
}

/* Separate captures preserve stock-compiler argument lifetimes. */
int CD_cw(int command, void *parameters, u8 *result, int mode)
{
  u8 *payload;
  u8 *parameter = parameters;
  int opcode;
  int i;

  opcode = command;
  if (D_8009AFC0 >= 2)
  {
    printf(D_80011BB4, D_8009AFDC[(u8) opcode]);
  }
  payload = parameter;
  i = 0;
  if (D_8009B1FC[(u8) opcode] && (!payload))
  {
    if (D_8009AFC0 > 0)
    {
      printf(D_80011BBC, D_8009AFDC[(u8) opcode]);
    }
    return -2;
  }
  CD_sync(i, 0);

  if (((u8) opcode) == 2)
  {
    for (i = 0; i < 4; i++)
    {
      D_8009AFD0[i] = payload[i];
    }

  }
  if (((u8) opcode) == 14)
  {
    D_8009AFD4 = payload[0];
  }
  {
    CdCommandTables *tables = &D_8009B0FC;
    D_8009B294.sync = 0;
    if (tables->ready_flags[(u8) opcode])
    {
      D_8009B294.ready = 0;
    }
    *D_8009B27C = 0;
    {
      int *counts = tables->parameter_counts;
      i = 0;
      if (counts[(u8) opcode] > 0)
      {
        int *count = &counts[(u8) opcode];
        do
        {
          *D_8009B284 = payload[i];
        }
        while ((++i) < (*count));
      }
    }
  }
  D_8009AFD5 = opcode;
  *D_8009B280 = opcode;
  if (mode)
  {
    return 0;
  }
  D_800A3478 = VSync(-1) + 0x3C0;
  D_800A347C = 0;
  D_800A3480 = D_80011BCC;
  if (!D_8009B294.sync)
  {
    char **commands = D_8009AFDC;
    char **events = D_8009B05C;
    u8 *sync = (u8 *) (&D_8009B294);
    u8 *ready = (u8 *)&((CdInterruptEvents *)sync)->ready;
    do
    {
      if (timed_out(commands, events, sync))
      {
        return -1;
      }
      if (CheckCallback())
      {
        dispatch_interrupts(sync, ready);
      }
    }
    while (!(*((volatile u8 *) sync)));
  }
  copy_result(result, D_800A3460);
  {
    unsigned long status = 0;
    unsigned short event = D_8009B294.sync;
    if (event == 5)
    {
      status = -1;
    }
    return status;
  }
}
