/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -fno-expensive-optimizations -mno-split-addresses */
#include "bios_internal.h"

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
    g_CdMode = payload[0];
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
  g_CdLastCom = opcode;
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
      if (timed_out(commands, events, (CdInterruptEvents *)sync))
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


int CD_vol(CdlATV *vol) {
    *g_CdRegIndexBase = 2;
    *g_CdRegDataWrite = vol->val0;
    *g_CdRegResponse = vol->val1;
    *g_CdRegIndexBase = 3;
    *g_CdRegPort1 = vol->val2;
    *g_CdRegDataWrite = vol->val3;
    *g_CdRegResponse = 0x20;
    return 0;
}

void CD_flush(void) {
    CdInterruptEvents *state;

    *g_CdRegIndexBase = 1;
    while (*g_CdRegResponse & 7) {
        *g_CdRegIndexBase = 1;
        *g_CdRegResponse = 7;
        *g_CdRegDataWrite = 7;
    }
    state = &D_8009B294;
    state->end = 0;
    state->ready = state->end;
    {
        volatile u8 *index = g_CdRegIndexBase;
        state->sync = 2;
        *index = 0;
    }
    *g_CdRegResponse = 0;
    *g_CdRegRequest = 0x1325;
}
