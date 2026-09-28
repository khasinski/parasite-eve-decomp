#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct BootClearRect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} BootClearRect;

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
void ClearImage(BootClearRect *rect, int r, int g, int b);
void DrawSync(int mode);
void Akao_Cmd_F1(void);
void Render_Noop(int mode);
void Asset_UnloadTableEntries(void);

void Boot_RunFrame(void)
{
    BootClearRect clear_rect;
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
        register u32 mask asm("$4") = ~0x3800u;
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
