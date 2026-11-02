#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/asset_tim.h"
#include "pe1/battle_status.h"
#include "pe1/boot.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_display.h"
#include "pe1/boot_services.h"
#include "pe1/cd_poll.h"
#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/disc_load_state.h"
#include "pe1/draw_buffers.h"
#include "pe1/draw_state.h"
#include "pe1/font.h"
#include "pe1/game_state.h"
#include "pe1/geom_state.h"
#include "pe1/menu_queue.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/render_camera.h"
#include "pe1/render_tint.h"
#include "pe1/save_tail_state.h"
#include "pe1/scene_assets.h"
#include "pe1/scene_dispatch.h"
#include "pe1/script_queue.h"
#include "include_asm.h"

/* Boot-time disc loads called from boot/main.c: the PE.IMG asset LBA
 * tables and the boot audio banks. */

void SetDispMask(int arg0);

int Draw_InitDisplay(int mode) {
    int mode_reg;
    int one;
    register u8 **slots asm("$18");
    register u8 *ptr asm("$2");
    u8 *ptr2;
    register int const_8000 asm("$4");
    int const_48000;
    RECT rect;
    u8 *tmp;

    mode_reg = mode;
    VSync(0);
    SetDispMask(0);
    PutDispEnv(g_RenderDispEnvArray);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0x1C0;
    one = 1;
    ClearImage(&rect, 0, 0, one);
    DrawSync(0);

    const_48000 = 0x48000;
    slots = g_OtBufferTable;
    asm volatile("" : "=r"(slots) : "0"(slots));

    ptr = g_BootWorkArea;
    ptr2 = ptr + 0x1800;
    g_SceneBankAssetSource = ptr;
    ptr += 0x6000;
    g_VoiceBankBase = ptr2;
    g_EntityBankBuffer = ptr;
    ptr += 0xE000;
    g_EntityTextureBlob = ptr;

    ptr = D_8010B7C0;
    ptr2 = D_801207C8;
    g_GameState.texture_load_scratch = (union SceneAssetView *)ptr;
    ptr = ptr2 + 0x1C98;
    g_SceneObjectWork = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable[0] = ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt[0] = ptr2;
    ptr2 = ptr + 0x1C00;
    g_GameState.object_texture_blob = (struct SceneAssetBlob *)ptr;
    ptr += 0x3800;
    g_RenderScratchBufferBase[0] = ptr;
    ptr += const_48000;
    g_ObjectTextureBlobMidWriteOnly = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase[0] = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase[0] = ptr2;
    ptr2 = ptr + 0x3800;
    g_ScriptSpriteBuffer = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase[0] = ptr;
    ptr += 0x7000;
    g_ScenePmSlotBanks = ptr;
    g_SceneLoadScratchBuffer[0] = g_SceneLoadScratchArea;
    g_LoadedSceneAssetBlock[0] = tmp - 8;
    g_StrFileDirectory = tmp;

    Draw_InitBuffers();
    CdRom_SetSeekPos(2);

    do {
        ClearOTagR(slots[g_ActiveDrawSlot], 0x1000);
        Render_SetCDDCSlot();
        Draw_RenderFrame();
    } while ((g_ScreenTint.fadeMode & 3) != one);

    Menu_ConsumeEquipSlotFlag();

    if (mode_reg == 1) {
        g_SceneDispatchToken = 0xA80830C8;
    } else if (mode_reg == 3) {
        g_SceneDispatchToken = 0xA80651C8;
    }
    return 0;
}

inline int Draw_GetDrawEnabled(void) {
    return g_DrawEnabled.value;
}

inline short Seq_GetElapsed(void) {
    if (g_DrawEnabled.bits != 0) {
        return g_SeqElapsed;
    }
    return -1;
}

/* Both queries are integrated here; the inlined calls leave retail's
 * otherwise empty eight-byte frame and the repeated enabled test. */
int Draw_CheckDrawStatus(void) {
    if (Draw_GetDrawEnabled()) {
        if (Seq_GetElapsed() > 0) {
            if (g_GameState.scene_audio.tracks.movie_wide != 0) {
                return 2;
            }
            return 1;
        }
    }
    return 0;
}

TimFile *Draw_GetTimTableEntry(TimRelativeTable *table, int index) {
    return (TimFile *)&table->bytes[table->offsets[(s16)index]];
}

void Draw_LoadTimTable(TimRelativeTable *table, int count) {
    int i;

    for (i = 0; i < count; i++) {
        u32 offset = table->offsets[(s16)i];
        Asset_LoadTimImage((TimFile *)&table->bytes[offset]);
    }
}

int Boot_LoadOverlayTables(void) {
    int status;
    int imageBase;
    Pe1GameState *state;
    u16 *range;
    TimRelativeTable *main_table;
    TimRelativeTable *large_table;
    int i;

    imageBase = g_PeImageBaseLba;

    VSync(0);
    SetDispMask(0);
    state = &g_GameState;
    CdRom_ReadAssetSectors(1, 0xCC, 0, g_SceneLoadScratchBuffer[0], 0x21, 1);

retry_first_load:
    range = g_MainAssetLbaTbl;
    do {
        status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->scene_object_work, range[1] - range[0]);
    } while (status == -1);
    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_first_load;
        }
    }

    main_table = (TimRelativeTable *)state->scene_object_work;
    for (i = 0; i < 3; i++) {
        Asset_LoadTimImage(SceneAsset_ResolveOffset(main_table, main_table->offsets[(s16)i]));
    }

    if (g_ScriptVariableBank[SCRIPT_VAR_FLAGS] & 0x2000) {
retry_large_load_a:
        range = g_LargeTexLbaTbl;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->object_texture_blob, range[1] - range[0]);
        } while (status == -1);
        while ((status = CdRom_PollReady()) != 0) {
            if (status == -1) {
                goto retry_large_load_a;
            }
        }
    } else {
retry_large_load_b:
        range = g_SmallTexLbaTbl;
        do {
            status = CdRom_ReadSectors(imageBase + range[0], 0, (int)state->object_texture_blob, range[1] - range[0]);
        } while (status == -1);
        while ((status = CdRom_PollReady()) != 0) {
            if (status == -1) {
                goto retry_large_load_b;
            }
        }
    }

    large_table = (TimRelativeTable *)state->object_texture_blob;
    for (i = 0; i < 0x106; i++) {
        Asset_LoadTimImage(SceneAsset_ResolveOffset(large_table, large_table->offsets[(s16)i]));
    }

    DrawSync(0);

retry_final_load:
    range = g_FontUiLbaTbl;
    do {
        status = CdRom_ReadSectors(g_PeImageBaseLba + range[0], 0, g_StrFileDirBuffer, range[1] - range[0]);
    } while (status == -1);
    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_final_load;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    return 0;
}

int CdRom_LoadBootAudio(void) {
    int status;
    u16 *range;
    int retry;

    g_GameState.scene_audio.tracks.keys[0][1] = -1;
    g_GameState.scene_audio.tracks.keys[0][0] = -1;
    g_GameState.scene_audio.tracks.keys[1][1] = -1;
    g_GameState.scene_audio.tracks.keys[1][0] = -1;
    g_GameState.scene_audio.tracks.banks[1] = -1;
    g_GameState.scene_audio.tracks.banks[0] = -1;
    g_GameState.flags &= -0xF1;
    Akao_Cmd_F0();

retry_first:
    range = g_SceneInitLbaTbl0;
    retry = -1;
    do {
        status = CdRom_ReadSectors(g_DiscLoadState.baseLba + range[0], 0, g_MdecOverlayLoadBase, range[1] - range[0]);
    } while (status == retry);

    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_first;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();

retry_second:
    range = &g_BootDisplayDataLbaTbl;
    retry = -1;
    do {
        status = CdRom_ReadSectors(g_DiscLoadState.baseLba + range[0], 0, g_CdLoadAreaBase, range[1] - range[0]);
    } while (status == retry);

    while ((status = CdRom_PollReady()) != 0) {
        if (status == -1) {
            goto retry_second;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    return 0;
}
