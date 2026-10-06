/* Boot-time system initialization: InitSystem, the subsystem and global
 * reset it is followed by, the start-up display reset, the vsync callback and
 * the pad start-up (G0341/G0342). */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_spu_api.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_gpu.h"
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
u32 Task_GpuFlushPrimQueue(void);
M2C_UNK Save_InitSystem();
M2C_UNK Task_InitNodePool();
M2C_UNK Entity_ResetStateGlobals();
M2C_UNK Entity_ResetAllPools();
M2C_UNK SsInit();
void Menu_SetEquipSlotIndex(int index);
void Boot_BuildRenderFlagTable(void);
M2C_UNK Save_PostInitStub();
M2C_UNK Task_ClearSfxTable();
M2C_UNK CdRom_InitScreenState();
M2C_UNK Task_InitGpuHwRegs();
extern int g_ActiveDrawSlot;
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
    Task_InitGpuHwRegs();
    for (var_s0 = 0; var_s0 < 0x7D0U; var_s0++) {
        Task_GpuFlushPrimQueue();
    }
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


extern u8 D_800BCDC8[];
extern u8 D_800BCE91;
extern u8 D_800BCEA5;
extern u16 D_800BCE9E;
extern u16 D_800BCE8A;
extern u16 D_800BCE9C;
extern u16 D_800BCE88;
extern u16 D_800BCEA2;
extern u16 D_800BCE8E;
extern u8 D_800BCE3C;
extern u8 D_800BCDE0;
extern u8 D_800BCE3A;
extern u8 D_800BCDDE;
extern u8 D_800BCE3B;
extern u8 D_800BCDDF;
extern u16 D_800BCE38;
extern u16 D_800BCDDC;
extern u8 D_800BCE3D;
extern u8 D_800BCDE1;
extern u8 D_800BCE3E;
extern u8 D_800BCDE2;
extern u8 D_800BCE3F;
extern u8 D_800BCDE3;

void ResetGraph(int arg0);
void SetGraphDebug(int level);
void SetDispMask(int mask);
void ClearImage(RECT *rect, int r, int g, int b);
DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);

void Render_ResetScene(int width, int height)
{
    s16 rect[4];
    u8 *disp;
    u8 *draw;

    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(0);

    rect[2] = 0x3FF;
    rect[3] = 0x200;
    rect[0] = 0;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 1);

    disp = &D_800BCE91;
    *disp = 1;
    D_800BCEA5 = 1;
    SetDefDispEnv(disp - 0x11, 0, height, width, height);
    SetDefDispEnv(disp + 3, 0, 0, width, height);

    draw = D_800BCDC8;
    D_800BCE9E = 8;
    D_800BCE8A = 8;
    D_800BCE9C = 0;
    D_800BCE88 = 0;
    D_800BCE8E = D_800BCEA2 = height;
    SetDefDrawEnv(draw, 0, 0, width, height);
    SetDefDrawEnv(draw + 0x5C, 0, height, width, height);

    D_800BCE3C = 1;
    D_800BCDE0 = 1;
    D_800BCE3A = 1;
    D_800BCDDE = 1;
    D_800BCE3B = 0;
    D_800BCDDF = 0;
    D_800BCE38 = 0;
    D_800BCDDC = 0;
    D_800BCE3D = 0;
    D_800BCDE1 = 0;
    D_800BCE3E = 0;
    D_800BCDE2 = 0;
    D_800BCE3F = 0;
    D_800BCDE3 = 0;
    g_ActiveDrawSlot = 0;

    PutDispEnv((DISPENV *)(disp - 0x11));
}

void Scene_TickTimers(void);

extern char g_AnalogStickState[];

void PadInitDirect(char *arg0, char *arg1);
void PadStartCom(void);

void Boot_VsyncCallback(void) {
    Task_GpuFlushPrimQueue();
    Scene_TickTimers();
}

void Boot_InitMemCard(void) {
    PadInitDirect(g_AnalogStickState, g_AnalogStickState + 0x22);
    PadStartCom();
}
