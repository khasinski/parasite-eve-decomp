/* CC1_FLAGS: -g3 -G8 -fno-schedule-insns2 */
/* MASPSX_FLAGS: -G8 */
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
extern struct { char _[16]; } D_8009CDDC_o __asm__("g_ActiveDrawSlot");
#define g_ActiveDrawSlot (*(s32 *)&D_8009CDDC_o)
extern struct { char _[16]; } D_8009D1A0_o __asm__("g_GameStateFlags");
#define g_GameStateFlags (*(s32 *)&D_8009D1A0_o)
extern struct { char _[16]; } D_8009D1C4_o __asm__("g_SceneDispatchCur");
#define g_SceneDispatchCur (*(s32 *)&D_8009D1C4_o)
extern struct { char _[16]; } D_8009D250_o __asm__("g_FrameRngCounter");
#define g_FrameRngCounter (*(s32 *)&D_8009D250_o)
extern struct { char _[16]; } D_8009D280_o __asm__("g_SceneDispatchToken");
#define g_SceneDispatchToken (*(s32 *)&D_8009D280_o)
extern struct { char _[16]; } func_8003E91C_o __asm__("Boot_VsyncCallback");
#define Boot_VsyncCallback (*(M2C_UNK *)&func_8003E91C_o)

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
