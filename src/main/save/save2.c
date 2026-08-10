#include "common.h"
#include "pe1/memcard.h"
extern u8 g_MemCardFileBuffer[];
void bzero(void *dst, int len);
void MemCard_CloseAll(void);

void MenuWidget_NavScrollTo(int selected_base);
void Inv_SetActiveList(int arg0, int arg1);

int Save_StartReadSlot(int port, int slot)
{
  register u8 *buffer;
  MemCardPortState *state;
  int saved_slot;
  state = &g_MemCardPortStates[port];
  saved_slot = slot;
  if (state->present == 1)
  {
    MemCard_CloseAll();
 do { buffer = g_MemCardFileBuffer; } while (0);
    state->transferSize = 0x2000;
    state->retryCount = 10;
    state->pendingError = 2;
    state->managerState = 1;
    state->nextState = 5;
    state->transferData = buffer;
    state->selectedSlot = saved_slot;
    g_MemCardActiveState = state;
    g_MemCardActiveBytesRemaining = 0x2000;
    bzero(buffer, 0x2000);
  }
  return 0;
}

void Save_CancelUiFlow(void) {
    MenuWidget_NavScrollTo(0x26);
    MenuWidget_NavScrollTo(0x25);
    MenuWidget_NavScrollTo(0x24);
    Inv_SetActiveList(0xC, 0);
}
