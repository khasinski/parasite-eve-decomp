
typedef unsigned char u8;
extern u8 D_800A0EDC[];
extern u8 D_800A12EC[];
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
  state = D_800A12EC;
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
