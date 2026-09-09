#include "pe1/memcard.h"
extern int D_800A12F8;
extern int D_800A0EE0;
extern int g_MemCardServicedPort;
extern int g_MemCardInfoPollCountdown;
extern int g_MemCardConnectDebounce;
extern int D_800A184C;
extern int D_800A1848;
extern int g_McOpPending;
extern int g_MemCardActivePortOneBased;
extern int g_MemCardSavePollTimeout;
extern int g_MemCardActivePromptPending;
extern int g_SaveTitleStyleFlag;
extern int g_MemCardLoadSucceeded;
extern int g_MemCardReadContext;

void bzero(void *ptr, int size);

void MemCard_InitState(void) {
    bzero(g_MemCardPortStates, sizeof(MemCardPortState) * 2);
    D_800A12F8 = -1;
    D_800A0EE0 = -1;
    g_MemCardServicedPort = 0;
    g_MemCardInfoPollCountdown = 0;
    g_MemCardConnectDebounce = 0;
    D_800A184C = 0;
    D_800A1848 = 0;
    g_McOpPending = 0;
    g_MemCardActiveState = 0;
    g_MemCardActivePortOneBased = 0;
    g_MemCardSavePollTimeout = 0;
    g_MemCardActivePromptPending = 0;
    g_SaveTitleStyleFlag = 0;
    g_MemCardLoadSucceeded = 0;
    g_MemCardReadContext = 0;
}


#include "common.h"
extern u8 D_800A0EDC[];
extern u8 g_Slot2QuickerSave[];
extern int g_MemCardActivePortOneBased;
extern int g_MemCardSavePollTimeout;
extern int g_MemCardActivePromptPending;
void MemCard_StepPortState(int arg0);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void Menu_CloseNotificationDialogs(void);
void Menu_SetDeferredCallback(void (*callback)(void));
void Menu_DestroyMemCardProgressWidget(void);
void Menu_NavToSaveConfirmDialog(void);
void MemCard_UpdateSaveState(int port);
void MemCard_ClearActivePrompt(void);
void MemCard_StartActivePortRead(void);

#include "pe1/memcard.h"

void MemCard_UpdateSavePolling(void)
{
  int port;
  u8 *state;
  int timeout;
  int activePort;
  int activeState;
  MemCard_StepPortState(1);
  MemCard_StepPortState(0);
  if (g_MemCardSavePollTimeout != 0)
  {
    activePort = g_MemCardActivePortOneBased - 1;
    activeState = D_800A0EDC[activePort * 0x418];
    if (activeState == 4)
    {
      goto active_state_ok;
    }
    {
      register int activeStateReady;
      activeStateReady = 1;
      if (activeState == activeStateReady)
      {
        goto active_state_ok;
      }
    }
    g_MemCardSavePollTimeout = -2;
    active_state_ok:
    timeout = g_MemCardSavePollTimeout;

    timeout -= timeout > 0;
    g_MemCardSavePollTimeout = timeout;
    if (timeout <= 0)
    {
      Menu_DestroyMemCardProgressWidget();
      if (g_MemCardSavePollTimeout == (-1))
      {
        goto timeout_expired;
      }
      if (g_MemCardSavePollTimeout != 0)
      {
        goto timeout_cancel;
        port = 1;
      }
      Menu_CreateNotificationDialog(0x52, 0);
      Menu_SetDeferredCallback(MemCard_StartActivePortRead);
      goto timeout_prompt_pending;
      timeout_expired:
      Menu_CreateNotificationDialog(0x3C, 0);

      Menu_SetDeferredCallback(MemCard_ClearActivePrompt);
      timeout_prompt_pending:
      g_MemCardActivePromptPending = 1;

      goto timeout_clear;
      timeout_cancel:
      MemCard_ClearActivePrompt();

      timeout_clear:
      g_MemCardSavePollTimeout = 0;

      port = 1;
    }
  }
  port = 1;
  state = g_Slot2QuickerSave;
  do
  {
    if (g_MemCardActivePortOneBased == (port + 1))
    {
      if (((*state) & 1) == 0)
      {
        Menu_NavToSaveConfirmDialog();
        Menu_CloseNotificationDialogs();
        MemCard_ClearActivePrompt();
      }
    }
    MemCard_UpdateSaveState(port);
    port--;
    state -= 0x418;
  }
  while (port >= 0);
}

int MemCard_IsPortPresent(int port) {
    return g_MemCardPortStates[port].present & 1;
}


#include "pe1/memcard.h"

int close(int fd);

void MemCard_CloseAll(void) {
    unsigned char *base;
    unsigned char *state;
    unsigned char *end;
    int stateOpen;
    int stateBusy;
    int closedFd;
    int stateClosed;

    base = (unsigned char *)g_MemCardPortStates;
    if (base < base + 0x830) {
        stateOpen = 8;
        stateBusy = 10;
        closedFd = -1;
        stateClosed = 12;
        state = base + 1;
        end = base + 0x831;
        do {
            if ((*state == stateOpen) || (*state == stateBusy)) {
                close(*(volatile int *)(state + 0xB));
                *(volatile int *)(state + 0xB) = closedFd;
                *state = stateClosed;
            }
            state += 0x418;
        } while (state < end);
    }
}


#include "pe1/memcard.h"

void Menu_StepItemGrid2(void);

extern int g_MemCardActivePortOneBased;

int MemCard_CheckPresent(int port) {
    int present;

    present = g_MemCardPortStates[port].present & 4;
    if (present != 0) {
        if (g_MemCardActivePortOneBased == 0) {
            g_MemCardActivePortOneBased = port + 1;
            Menu_StepItemGrid2();
        }
    }
    return present == 0;
}

int MemCard_GetActivePort(void)
{
    return g_MemCardActivePortOneBased - 1;
}


#include "pe1/memcard.h"

void MemCard_MarkActivePortState13(void) {
    int portIndex = g_MemCardActivePortOneBased - 1;
    g_MemCardPortStates[portIndex].managerState = 0xD;
}


extern volatile int g_MemCardActivePortOneBased;
extern volatile int g_MemCardActivePromptPending;

void MemCard_ClearActivePrompt(void) {
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}


#include "common.h"
#include "pe1/memcard.h"
extern int g_MemCardActivePortOneBased;
extern int g_MemCardActivePromptPending;

void MemCard_StartRead(int port, int arg1);

extern int g_MemCardReadContext;

void MenuWidget_SaveAndSetCurrentNode(int arg0);

void MemCard_StartActivePortRead(void) {
    MemCard_StartRead(g_MemCardActivePortOneBased - 1, 1);
    g_MemCardActivePortOneBased = 0;
    g_MemCardActivePromptPending = 0;
}

int MemCard_GetPortSequence(int arg0) {
    return g_MemCardPortStates[arg0].sequence;
}

void MemCard_StartRead(int port, int arg1) {
    MemCardPortState *state = &g_MemCardPortStates[port];

    if (state->managerState == 0 || state->managerState == 12) {
        state->managerState = 1;
        state->nextState = 2;
        state->retryCount = 10;
        MenuWidget_SaveAndSetCurrentNode(0);
        g_MemCardReadContext = arg1;
    }
}


#include "pe1/memcard.h"
extern unsigned char D_800A12ED;
extern int g_McOpPending;

int close(int fd);

void MemCard_CloseAllAndResetState(void) {
    unsigned char *base;
    unsigned char *state;
    unsigned char *end;
    int stateOpen;
    int stateBusy;
    int closedFd;
    int stateClosed;

    base = (unsigned char *)g_MemCardPortStates;
    if (base < base + 0x830) {
        stateOpen = 8;
        stateBusy = 10;
        closedFd = -1;
        stateClosed = 12;
        state = base + 1;
        end = base + 0x831;
        do {
            if ((*state == stateOpen) || (*state == stateBusy)) {
                close(*(volatile int *)(state + 0xB));
                *(volatile int *)(state + 0xB) = closedFd;
                *state = stateClosed;
            }
            state += 0x418;
        } while (state < end);
    }

    D_800A12ED = 0;
    g_MemCardPortStates[0].managerState = 0;
    g_McOpPending = 0;
}


#include "pe1/memcard.h"

extern int g_McOpPending;

extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

int MemCard_IsPortTransferState(int arg0) {
    u8 value = g_MemCardPortStates[arg0].managerState;

    return (value == 3) || (value == 8) || (value == 10);
}

int MemCard_IsOperationPending(void) {
    return g_McOpPending;
}

void MemCard_ClearDelayedCallback(void) {
    g_MemCardDelayedCallback = 0;
    g_MemCardDelayedCallbackTimer = 0;
}


extern void (*g_MemCardDelayedCallback)(void);
extern int g_MemCardDelayedCallbackTimer;

void MemCard_SetDelayedCallback(void (*callback)(void)) {
    g_MemCardDelayedCallback = callback;
    g_MemCardDelayedCallbackTimer = 1;
}

void MemCard_DelayedCallback(void) {
    void (*callback)(void);

    callback = g_MemCardDelayedCallback;
    if (callback != 0) {
        g_MemCardDelayedCallbackTimer++;
        if (g_MemCardDelayedCallbackTimer == 4) {
            callback();
            g_MemCardDelayedCallback = 0;
            g_MemCardDelayedCallbackTimer = 0;
        }
    }
}


extern void (*g_MemCardDelayedCallback)(void);

extern int g_MemCardEventF400Spec0004Flag;
extern int g_MemCardRemovedEventPending;
extern int g_MemCardEventF400Spec2000Flag;
extern int g_MemCardEventF000Spec0004Flag;
extern int g_MemCardEventF000Spec8000Flag;
extern int g_MemCardEventF000Spec2000Flag;

int MemCard_HasDelayedCallback(void) {
    return g_MemCardDelayedCallback != 0;
}

int MemCard_OnEventF400Spec0004(void)
{
    g_MemCardEventF400Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF400Spec8000(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec0100(void)
{
    g_MemCardRemovedEventPending = 1;
    return 0;
}

int MemCard_OnEventF400Spec2000(void)
{
    g_MemCardEventF400Spec2000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0004(void)
{
    g_MemCardEventF000Spec0004Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec8000(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec0100(void)
{
    g_MemCardEventF000Spec8000Flag = 1;
    return 0;
}

int MemCard_OnEventF000Spec2000(void)
{
    g_MemCardEventF000Spec2000Flag = 1;
    return 0;
}
