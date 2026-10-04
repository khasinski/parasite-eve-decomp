#include "common.h"
#include "pe1/boot_disc_check.h"

/* Boot notice screen and disc check; mode 1 or 2 is the expected disc. */
int Render_InitDisplayLists(int mode)
{
    RECT rect;
    u8 result[8];
    short colors[8];
    int state;
    int done;
    int command;
    int wait;
    int status;

    state = 0;
    done = 0;
    command = -1;
    colors[0] = -1;
    wait = 180;
    VSync(0);
    SetDispMask(0);
    /* Recorded crutch debt: retail's CD retry loops restart through gotos,
     * not reproducible with structured loops in stock GCC 2.7.2 (loop.c
     * hoists the read constants out of the restart path). */
retryNotice:
    while (CdRom_ReadSectorsFromLba(g_GameState.pe_image_base_lba + D_800930D8[0],
               g_GameState.loaded_scene_assets, D_800930D8[1] - D_800930D8[0]) == -1)
        ;
    for (;;) {
        status = CdRom_PollReady();
        if (status == 0)
            break;
        if (status == -1)
            goto retryNotice;
    }
retryFog:
    while (CdRom_ReadSectorsFromLba(g_GameState.pe_image_base_lba + D_800930D8[1],
               g_GameState.scene_load_scratch, D_800930D8[2] - D_800930D8[1]) == -1)
        ;
    for (;;) {
        status = CdRom_PollReady();
        if (status == 0)
            break;
        if (status == -1)
            goto retryFog;
    }
    Boot_BuildRenderFlagTable();
    Render_SetupFogLayer(g_GameState.scene_load_scratch);
    rect.x = 0;
    rect.y = 0;
    rect.w = 320;
    rect.h = 448;
    ClearImage(&rect, 0, 0, 1);
    Gpu_LoadTimImage(g_GameState.loaded_scene_assets);
    rect.x = 320;
    rect.y = 256;
    rect.w = 320;
    rect.h = 224;
    DrawSync(0);
    PutDispEnv(&D_800BCE80[D_8009CDDC]);
    SetDispMask(1);
    while (!done) {
        ClearOTagR(D_800B0E38[D_8009CDDC], 0x1000);
        switch (state) {
        case 0:
            if (Cd_GetReadyStatus() == 1 && CdRom_GetPendingReadCount() == 0) {
                command = Render_AllocParticleNode(8, 0, 0, -1);
                state = 1;
            }
            break;
        case 1:
            status = Render_FindParticleEffect(command, result);
            switch (status) {
            case 2:
                Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
                state = 2;
                break;
            case 5:
            case 6:
                state = 0;
                break;
            }
            break;
        case 2:
            if (CdRom_GetCmdStatus() & 0x10)
                state = 3;
            break;
        case 3:
            status = Cd_GetReadyStatus();
            switch (status) {
            case 2:
                break;
            case 1:
                Tbl_ResetAll();
                Render_SetupColorTable(mode == 1 ? 3 : 4, 0, colors);
                wait = 180;
                state = 5;
                break;
            case 3:
                wait = 180;
                state = 6;
                break;
            }
            break;
        case 4:
            status = OpenPeImage();
            switch (status) {
            case -1:
                Tbl_ResetAll();
                wait = 180;
                Render_SetupColorTable(5, 0, colors);
                state = 7;
                break;
            case -2:
                Tbl_ResetAll();
                wait = 180;
                Render_SetupColorTable(5, 0, colors);
                state = 8;
                break;
            case 0:
                if ((mode == 1 && (D_800B0DCD & 1)) || (mode == 2 && (D_800B0DCD & 2))) {
                    done = 1;
                } else {
                    wait = 180;
                    state = 9;
                }
                break;
            }
            break;
        case 5:
            if (wait != 0)
                wait--;
            else
                state = 4;
            break;
        case 6:
            if (wait != 0) {
                wait--;
                break;
            }
            Tbl_ResetAll();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 7:
            if (wait != 0) {
                wait--;
                break;
            }
            Tbl_ResetAll();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 8:
            if (wait != 0) {
                wait--;
                break;
            }
            Tbl_ResetAll();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        case 9:
            if (wait != 0) {
                wait--;
                break;
            }
            Tbl_ResetAll();
            Render_SetupColorTable(mode == 1 ? 1 : 2, 0, colors);
            state = 2;
            break;
        }
        Menu_DrawTextboxEntries();
        DrawSync(0);
        VSync(0);
        Render_InitEntityPool(1);
        PutDispEnv(&D_800BCE80[D_8009CDDC]);
        PutDrawEnv(&D_800BCDC8[D_8009CDDC]);
        MoveImage(&rect, 0, D_8009CDDC ? 224 : 0);
        DrawOTag(&D_800B0E38[D_8009CDDC][0xFFF]);
        D_8009CDDC ^= 1;
    }
    VSync(0);
    SetDispMask(0);
    rect.x = 0;
    rect.y = 0;
    rect.w = 320;
    rect.h = 448;
    ClearImage(&rect, 0, 0, 1);
    DrawSync(0);
    D_800B0DCD = mode == 1 ? 1 : 2;
    while (Cd_GetReadyStatus() != 1)
        VSync(0);
    D_800B0DD4 = CdRom_GetDiskType();
    return 0;
}
