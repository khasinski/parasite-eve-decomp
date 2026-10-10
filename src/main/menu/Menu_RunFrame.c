#include "pe1/menu_background.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_widget.h"
#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

void Window_SetBoundsByMode(int mode);
void MemCard_UpdateSavePolling(void);
void MemCard_DelayedCallback(void);
void Evt_DeferredExec(void);
void Menu_ProcessSwapReturnIfPending(void);
extern s32 g_MemCardDialogState;
u32 g_MenuErrorSoundPending;
extern s32 g_MenuActiveListTarget[];
#define g_MenuActiveListTarget (g_MenuActiveListTarget[0])

/* Queued by frontend commands, consumed by either frame entry point. */
void Menu_RequestErrorSound(void) {
    g_MenuErrorSoundPending = 1;
}

s32 Menu_RunFrameWithArg(s32 arg0) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = arg0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Evt_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Window_SetBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

s32 Menu_RunFrame(void) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = 0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Evt_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Window_SetBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}
