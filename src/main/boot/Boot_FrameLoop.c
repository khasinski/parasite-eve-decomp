/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/render_camera.h"
#include "pe1/psyq_gpu.h"
#include "pe1/game_state.h"

extern s32 g_GlobalFrameCounter;
extern char D_8009D224[];
extern s32 g_SceneDispatchToken;
extern char g_TaskNodeSeqCounter[];
extern char D_800B0CE4[];
extern char D_800B1628[];
extern char D_800B162C[];
extern char D_800B8A18[];
extern char g_ScreenTransitionState[];
extern char D_800BEA40[];
extern char D_800BEA42[];
extern char D_800BEA44[];
extern char D_800BEA46[];
extern char D_800BEA48[];
extern char D_800BEA4A[];
extern char D_800BEA4C[];
extern char D_800BEA4E[];
extern char D_800BEA50[];
extern char D_800942EC[];

extern s32 g_FieldMoveLock;

#define W(sym) (*(s32 *)(sym))
#define H(sym) (*(s16 *)(sym))
#define B(sym) (*(s8 *)(sym))
#define UB(sym) (*(u8 *)(sym))
#define LAUNDER(x) asm volatile("" : "=r"(x) : "0"(x))
#define BARRIER() 

void Akao_LoadVoiceBankAlt(void);
void Scene_LoadRoom(int arg0);
s32 Overlay_StreamTexturePage(void);
void Entity_InitFreePool(void);
void Task_InitNodeFreeList(void);
void Gte_SetBackColor(void *arg0, int arg1, int arg2, int arg3);
void Gte_SetLightColor(void *arg0, int arg1, int arg2, int arg3, int arg4);
int CdRom_DetectDiscChange(void);
int Scene_LoadEntityTexture(void);
void Scene_SetStoryDay(s8 storyDay);
int Scene_LoadEntityTextures(void);
void Entity_RelocateSceneData(void);
void Render_SetupFogLayer(int arg0);
void Task_DrawSyncAndFlush(void);
void func_800E0060(void);
void SetDispMask(int arg0);

void Gpu_InitPipeline(void) {
    int one;
    int v1;
    register int t0 asm("$8");
    register int t1 asm("$9");
    char *p;
    char *a0p;
    int geom;
    int frame_pad[6];

    Akao_LoadVoiceBankAlt();
    Scene_LoadRoom(g_SceneDispatchToken);
    Overlay_StreamTexturePage();
    W(D_8009D224) = 1;
    H(g_TaskNodeSeqCounter) = 1;
    Entity_InitFreePool();
    Task_InitNodeFreeList();

    Gte_SetBackColor(D_800BEA42 - 2, 0x28, 0x28, 0x28);
    BARRIER();

    v1 = 0;
    t1 = 0;
    LAUNDER(v1);
    if (v1 < -0x7FFF) {
        v1 = -0x7FFF;
    }
    t0 = 0x7FFF;
    a0p = D_800BEA40;
    *(s16 *)a0p = t0;
    H(D_800BEA42) = t1;
    H(D_800BEA44) = v1;
    Gte_SetLightColor(a0p, 0, 0xFF, 0xFF, 0xFF);

    v1 = 0;
    t0 = 0;
    LAUNDER(v1);
    t1 = 0x7FFF;
    if (v1 < -0x7FFF) {
        v1 = -0x7FFF;
    }
    p = D_800BEA46;
    *(s16 *)p = t0;
    H(D_800BEA48) = t1;
    H(D_800BEA4A) = v1;
    Gte_SetLightColor(p - 6, 1, 0x80, 0x80, 0x80);

    v1 = 0x10000;
    t0 = 0;
    {
        register int cond asm("$2");
        cond = 1;
        LAUNDER(cond);
        t1 = 0;
        if (cond != 0) {
            v1 = 0x7FFF;
        } else {
            LAUNDER(v1);
            if (v1 < -0x7FFF) {
                v1 = -0x7FFF;
            }
        }
    }
    p = D_800BEA4C;
    *(s16 *)p = t0;
    H(D_800BEA4E) = t1;
    H(D_800BEA50) = v1;
    Gte_SetLightColor(p - 0xC, 2, 0x60, 0x60, 0x60);

    one = 1;
    Render_PrepareFrame();
    SetGeomScreen(W(D_800B8A18));
    UB(g_ScreenTransitionState) |= 0x40;
    CdRom_DetectDiscChange();
    while (Scene_LoadEntityTexture() == one) {
    }

    one = 1;
    Scene_SetStoryDay(B(D_800B0CE4));
    while (Scene_LoadEntityTextures() == one) {
    }

    Entity_RelocateSceneData();
    if (g_GameState.flags & 0x40000000) {
        geom = W(D_800B162C);
    } else {
        geom = W(D_800B1628);
    }
    Render_SetupFogLayer(geom);
    Task_DrawSyncAndFlush();
    func_800E0060();
    g_GlobalFrameCounter = 0;
    H(D_800942EC) = 0;
    DrawSync(0);
    SetDispMask(1);
    {
        int tmp;
        v1 = ~0x40;
        tmp = g_GameStateFlags;
        a0p = (char *)&g_GameState;
        g_GameStateFlags = tmp & v1;
    }
    asm volatile("" : : : "memory");
    {
        /* g_FieldMoveLock is at 0x8009D2E8 in the USA image. */
        u32 load_page = 0x800A0000u;
        u32 store_page;
        u32 flags;
        flags = *(volatile u32 *)(load_page - 0x2D18u);
        flags &= ~0xCu;
        store_page = 0x800A0000u;
        *(volatile u32 *)(store_page - 0x2D18u) = flags;
    }
    W(a0p) &= ~0x402;
}

/* Field frame loop and its scene flush: Boot_FlushSceneFast is the exit tail
 * that Boot_RunFrame repeats after its loop. Contiguous -G8 pair. */
extern u32 D_8009D1C4;
extern u32 D_8009D280;
extern u8 D_8009CDD8_blob[] asm("D_8009CDD8");
#define D_8009CDD8 (*(u32 *)D_8009CDD8_blob)
extern u32 D_8009D1A0;
extern u32 D_8009CDA4;
extern u8 D_8009D1F4_blob[] asm("D_8009D1F4");
#define D_8009D1F4 (*(u32 *)D_8009D1F4_blob)
extern u8 D_8009D238_blob[] asm("D_8009D238");
#define D_8009D238 (*(u32 *)D_8009D238_blob)
extern u8 D_8009CDDC_blob[] asm("D_8009CDDC");
#define D_8009CDDC (*(u32 *)D_8009CDDC_blob)
/* Independent views preserve the original load-then-store sequence. */
extern u32 D_8009D250_read[] asm("D_8009D250");
extern u32 D_8009D250_write[] asm("D_8009D250");
extern u8 D_8009D26C_blob[] asm("D_8009D26C");
#define D_8009D26C (*(u32 *)D_8009D26C_blob)
/* These views prevent GCC from retaining one address across frame phases. */
extern u32 D_800B0CD8_post[3] asm("D_800B0CD8");
extern u32 D_800B0CD8_check[3] asm("D_800B0CD8");
extern u32 D_800B0CD8_read[3] asm("D_800B0CD8");
extern u32 D_800B0CD8_write1[3] asm("D_800B0CD8");
extern u32 D_800B0CD8_write2[3] asm("D_800B0CD8");
extern u8 D_800B0CEA[];
extern u8 D_800BCFE8[];

void Gpu_InitPipeline(void);
void Field_HandleStateTransition(void);
void ClearOTagR(u32 *address, int length);
void Scene_UpdateEntityList(void);
void Entity_FrameUpdate(void);
int Gpu_CheckDrawStatus(void);
int func_80122040(void);
void func_80121A00(void);
void Gpu_ClearOnFlag(void);
void Render_Update(void);
void Menu_DrawTextboxEntries(void);
void Render_SetGteScreenOffset(void);
void func_800E01BC(void);
void Render_ResetGteScreenOffset(void);
void Render_SetCDDCSlot(void);
void Gpu_RenderFrame(void);
void Render_SetFadeColour(int amount);
int VSync(int mode);
void Sys_Shutdown(void);
void Akao_StepVoiceTable(void);
void ClearImage(RECT *rect, int r, int g, int b);
void DrawSync(int mode);
void Akao_Cmd_F1(void);
void Render_Noop(int mode);
void Asset_UnloadTableEntries(void);

void Boot_FlushSceneFast(void) {
    RECT rect;
    u32 status;
    u32 first_status;

    if (g_GameState.flags & 0x200) {
        rect.w = 0x140;
        rect.x = 0;
        rect.y = 0;
        rect.h = 0x1C0;
        ClearImage(&rect, 0, 0, 1);
    }
    DrawSync(0);
    Akao_Cmd_F1();
    Render_Noop(1);
    Asset_UnloadTableEntries();
    status = g_GameState.flags | 2;
    g_GameStateFlags = (g_GameStateFlags | 0x40) & ~0x3800;
    first_status = status & ~0x800;
    g_GameState.flags = first_status;
    if (status & 0x200) {
        g_GameState.flags = (first_status | 2) & 0xFFFF7DFF;
    }
}

void Boot_RunFrame(void)
{
    RECT clear_rect;
    u32 flags;
    u32 status;
    u32 *game_flags;
    u8 *scene_flags;
    u8 *fade;

    Gpu_InitPipeline();
    if (D_8009D1C4 == D_8009D280) {
        scene_flags = D_800B0CEA;
        game_flags = (u32 *)(scene_flags - 0x12);
        fade = D_800BCFE8;
        do {
            D_8009CDD8 = 0;
            *scene_flags = 0;
            Field_HandleStateTransition();
            flags = D_8009D1A0;
            D_8009D1A0 = flags & ~0x30u;

            if (!(*(u32 *)(scene_flags - 0x12) & 0x8000) && D_8009CDA4 &&
                (D_8009D1F4 & 4) &&
                !(D_8009D238 & 0xB0002380u)) {
                D_8009D1A0 =
                    ((flags & 1) ? (flags & ~0x30u) | 0x20u
                                 : (flags & ~0x30u) | 0x10u) ^ 1u;
            }

            if (D_8009D1A0 & 1)
                goto do_vsync;
            if (!(*game_flags & 0x200)) {
                ClearOTagR(((u32 **)((u8 *)game_flags + (D_8009CDDC << 2)))[0x58],
                           0x1000);
            }
            D_8009D250_write[0] = D_8009D250_read[0] + 1;
            Scene_UpdateEntityList();
            Entity_FrameUpdate();

            if (!(*game_flags & 0x100)) {
                if ((s8)Gpu_CheckDrawStatus() != 0) {
                    if ((s8)func_80122040() == 0) {
                        func_80121A00();
                        Gpu_ClearOnFlag();
                        goto loop_end_check;
                    }
                } else if (!(*game_flags & 0x200)) {
                    Render_Update();
                    Menu_DrawTextboxEntries();
                    Render_SetGteScreenOffset();
                    func_800E01BC();
                    Render_ResetGteScreenOffset();
                    Render_SetCDDCSlot();
                }
                Gpu_RenderFrame();
                if (!D_8009CDA4 && *(u32 *)fade == 0xFF00FFu &&
                    *(s16 *)(fade + 4) == 0xFF && (fade[6] & 0x40)) {
                    Render_SetFadeColour(15);
                }
            }
            goto after_vsync;
do_vsync:
            VSync(2);
after_vsync:;

            if (!(D_800B0CD8_post[0] & 0x4200) &&
                (D_8009D26C & 0x0F000006u) == 0x0F000006u) {
                Sys_Shutdown();
            }
            if (!(*game_flags & 0x100)) {
                Akao_StepVoiceTable();
            }
            D_8009CDA4++;
            if ((D_8009D1A0 & 0x2000) && (*game_flags & 0x800)) {
                break;
            }
loop_end_check:
            ;
        } while (D_8009D1C4 == D_8009D280);
    }

    if (D_800B0CD8_check[0] & 0x200) {
        clear_rect.x = 0;
        clear_rect.y = 0;
        clear_rect.w = 0x140;
        clear_rect.h = 0x1C0;
        ClearImage(&clear_rect, 0, 0, 1);
    }
    DrawSync(0);
    Akao_Cmd_F1();
    Render_Noop(1);
    Asset_UnloadTableEntries();
    {
        u32 mask = ~0x3800u;
        register u32 final_flags asm("$2");
        final_flags = (D_8009D1A0 | 0x40) & mask;
        status = D_800B0CD8_read[0] | 2;
        flags = final_flags;
    }
    D_8009D1A0 = flags;
    {
        u32 first_status = status & ~0x800u;
        D_800B0CD8_write1[0] = first_status;
        if (status & 0x200)
            D_800B0CD8_write2[0] = (first_status | 2u) & ~0x8200u;
    }
}
