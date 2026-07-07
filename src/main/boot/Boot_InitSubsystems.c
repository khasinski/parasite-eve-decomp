/* CC1_FLAGS: -fno-schedule-insns2 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Save_InitSystem();
M2C_UNK VSyncCallback();
M2C_UNK Task_InitNodePool();
M2C_UNK Entity_ResetStateGlobals();
M2C_UNK Entity_ResetAllPools();
M2C_UNK ssinit();
M2C_UNK Menu_SetEquipSlotIndex();
M2C_UNK Menu_ConsumeEquipSlotFlag();
M2C_UNK Boot_BuildRenderFlagTable();
M2C_UNK Save_PostInitStub();
M2C_UNK Task_ClearSfxTable();
M2C_UNK CdRom_InitScreenState();
M2C_UNK Task_InitGpuHwRegs();
M2C_UNK Task_GpuFlushPrimQueue();
extern s32 g_ActiveDrawSlot[];
#define g_ActiveDrawSlot (g_ActiveDrawSlot[0])
extern s32 g_GameStateFlags[];
#define g_GameStateFlags (g_GameStateFlags[0])
extern s32 g_SceneDispatchCur[];
#define g_SceneDispatchCur (g_SceneDispatchCur[0])
extern s32 g_FrameRngCounter[];
#define g_FrameRngCounter (g_FrameRngCounter[0])
extern s32 g_SceneDispatchToken[];
#define g_SceneDispatchToken (g_SceneDispatchToken[0])
extern M2C_UNK Boot_VsyncCallback[];
#define Boot_VsyncCallback (Boot_VsyncCallback[0])

void Boot_InitSubsystems(void) {
    u32 var_s0;

    g_SceneDispatchCur = 0;
    g_SceneDispatchToken = 0;
    g_GameStateFlags = 0;
    g_FrameRngCounter = 0;
    g_ActiveDrawSlot = 0;
    var_s0 = 0;
    Task_InitGpuHwRegs();
    do {
        var_s0 += 1;
        Task_GpuFlushPrimQueue();
    } while (var_s0 < 0x7D0U);
    Boot_BuildRenderFlagTable();
    ssinit();
    VSyncCallback(NULL);
    VSyncCallback(&Boot_VsyncCallback);
    Menu_SetEquipSlotIndex(0);
    Save_InitSystem();
    Save_PostInitStub();
    CdRom_InitScreenState();
    Task_InitNodePool();
    Entity_ResetStateGlobals();
    Entity_ResetAllPools();
    Task_ClearSfxTable();
    Menu_ConsumeEquipSlotFlag();
}
