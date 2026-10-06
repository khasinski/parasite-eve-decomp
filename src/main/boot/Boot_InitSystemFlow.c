/* CC1_FLAGS: -fno-schedule-insns2 */
/* Boot-time system initialization: InitSystem and the subsystem and global
 * reset it is followed by. Contiguous pair; -fno-schedule-insns2 is the
 * subsystem reset's option and leaves InitSystem unchanged. The display
 * reset that follows does change under it and stays separate. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_spu_api.h"
#include "pe1/psyq_callbacks.h"
#include "common.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/font.h"

void Render_ResetScene(int arg0, int arg1);
void InitGeom(void);
void SetGeomOffset(int x, int y);
void SetGeomScreen(int h);
void MemCard_InitManager(void);
void Boot_InitMemCard(void);
void DsInit(void);

void InitSystem(void) {
    ResetCallback();
    Render_ResetScene(0x140, 0xE0);
    SpuInit();
    InitGeom();
    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(0xF0);
    MemCard_InitManager();
    Boot_InitMemCard();
    DsInit();
    DsSetDebug(0);
}

#define NULL ((void *)0)
void Task_GpuFlushPrimQueue(void);
M2C_UNK Save_InitSystem();
M2C_UNK Task_InitNodePool();
M2C_UNK Entity_ResetStateGlobals();
M2C_UNK Entity_ResetAllPools();
M2C_UNK SsInit();
M2C_UNK Menu_SetEquipSlotIndex();
M2C_UNK Boot_BuildRenderFlagTable();
M2C_UNK Save_PostInitStub();
M2C_UNK Task_ClearSfxTable();
M2C_UNK CdRom_InitScreenState();
M2C_UNK Task_InitGpuHwRegs();
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
void Boot_VsyncCallback(void);

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
    SsInit();
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
