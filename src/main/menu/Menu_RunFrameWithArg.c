/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Window_SetBoundsByMode(u8);                 /* extern */
M2C_UNK MemCard_UpdateSavePolling();                  /* extern */
M2C_UNK MemCard_DelayedCallback();                            /* extern */
s32 Menu_SaveBgIsFadeActive();                                /* extern */
M2C_UNK Menu_SaveBgAdvanceFade();                            /* extern */
M2C_UNK Evt_DeferredExec();                            /* extern */
M2C_UNK Menu_ProcessSwapReturnIfPending();                            /* extern */
M2C_UNK Inv_SetActiveList(M2C_UNK, M2C_UNK);            /* extern */
s32 Menu_GetCommandResult();                                /* extern */
M2C_UNK Menu_ClearCommandResult();                            /* extern */
M2C_UNK MenuInput_DispatchQueuedEvents();                            /* extern */
M2C_UNK Draw_SelectBuffer();                            /* extern */
M2C_UNK Draw_PresentFrame(M2C_UNK);                     /* extern */
M2C_UNK MenuWidget_UpdateAndDraw();                            /* extern */
extern u8 g_SavedMenuMode;
extern s32 g_MemCardDialogState;
extern s32 g_MenuErrorSoundPending;
extern s32 g_MenuActiveListTarget[];
#define g_MenuActiveListTarget (g_MenuActiveListTarget[0])

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
            Window_SetBoundsByMode(g_SavedMenuMode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}
