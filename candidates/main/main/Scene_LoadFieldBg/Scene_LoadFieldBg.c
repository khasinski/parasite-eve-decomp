#include "common.h"
#include "pe1/game_state.h"
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_tim.h"
#include "pe1/menu_memcard_display.h"
#include "pe1/render_prim.h"
#include "pe1/field_bg_load.h"

void Akao_Cmd_F1(void);
void DrawSync(int mode);
void Render_InitEntityPool(int mode);
void SetDispMask(int mask);
void Battle_DrawHPBar(void);

/* Uploads every TIM listed in a scene blob's directory. */
#define FIELD_BG_LOAD_BLOB_TIMS(source)                                         \
    {                                                                        \
        unsigned int i;  \
                                                                             \
        blob = (source);                                                     \
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);   \
        tim = SceneAsset_ResolveOffset(blob, directory->timEntries & 0x3fffff); \
        for (i = 0; i < directory->timEntries >> 22; i++)                    \
            Gpu_LoadTimAsset(&tim[i], blob);                                 \
    }

int Scene_LoadFieldBg(void)
{
    Pe1GameState *state = &g_GameState;
    u32 lba;
    unsigned int slot;
    FieldBgImageRecord *image;
    SceneAssetBlob *blob;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    int status;
    int done;

    if (!(state->flags & 1))
        return 0;

    lba = g_PeImageBaseLba;

retry1:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[9], state->bg_texture_blob,
               D_800930D8[10] - D_800930D8[9]) == -1)
        ;
    status = 1;
    do {
        if (status == -1)
            goto retry1;
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry2:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[10], state->bg_tim,
               D_800930D8[11] - D_800930D8[10]) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            FIELD_BG_LOAD_BLOB_TIMS(state->bg_texture_blob);
            for (slot = 2; slot < 4; slot++) {
                D_80091648[slot].tpage = FIELD_BG_TPAGE(D_80091648[slot].x, D_80091648[slot].y);
                D_80091648[slot].clut = FIELD_BG_CLUT(D_80091648[slot].clut_x, D_80091648[slot].clut_y);
            }
            image = Asset_FindTable08ByU32Key(blob, 0xABADC06C);
            while (image->length != 0) {
                LoadImage(&image->rect, image->pixels);
                image = (FieldBgImageRecord *)(&image->length + (image->length >> 2));
            }
            done = 1;
        }
        if (status == -1)
            goto retry2;
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry3:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[11], state->hud_tim,
               D_800930D8[12] - D_800930D8[11]) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            Gpu_LoadTimImage(state->bg_tim);
            for (slot = 0; slot < 2; slot++) {
                D_80091648[slot].tpage = FIELD_BG_TPAGE(D_80091648[slot].x, D_80091648[slot].y);
                D_80091648[slot].clut = FIELD_BG_CLUT(D_80091648[slot].clut_x, D_80091648[slot].clut_y);
            }
            done = 1;
        }
        if (status == -1)
            goto retry3;
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry4:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[12], state->bank_asset_source,
               D_800930D8[13] - D_800930D8[12]) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            done = 1;
            Gpu_LoadTimImage(state->hud_tim);
            Battle_DrawHPBar();
        }
        if (status == -1)
            goto retry4;
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry5:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[4], state->object_texture_blob,
               D_800930D8[5] - D_800930D8[4]) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            done = 1;
            state->object_placement_table =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0xC4B5BA04);
            state->room_geometry_table =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0xCAAD0704);
            state->bank_asset_table =
                Asset_FindTable08ByU32Key(state->bank_asset_source, 0x5EAF6804);
        }
        if (status == -1)
            goto retry5;
        status = CdRom_PollReady();
    } while (status != 0);

    done = 0;
retry6:
    while (CdRom_ReadSectorsFromLba(lba + D_800930D8[39], state->scene_process_slots,
               D_800930D8[40] - D_800930D8[39]) == -1)
        ;
    status = 1;
    do {
        if (!done) {
            FIELD_BG_LOAD_BLOB_TIMS(state->object_texture_blob);
            done = 1;
        }
        if (status == -1)
            goto retry6;
        status = CdRom_PollReady();
    } while (status != 0);

    {
        SceneAssetBlob *blob = state->scene_process_slots;
        unsigned int i;

        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        tim = SceneAsset_ResolveOffset(blob, directory->timEntries & 0x3fffff);
        i = 0;
        if (directory->timEntries >> 22) {
            do {
                Gpu_LoadTimAsset(&tim[i], blob);
                i++;
            } while (i < directory->timEntries >> 22);
        }
    }

    Akao_Cmd_F1();
    DrawSync(0);
    Render_InitEntityPool(1);
    VSync(0);
    PutDispEnv(&D_800BCE80[D_8009CDDC]);
    SetDispMask(1);
    state->field_bg_cache_id = -1;
    state->loaded_entity_bank = 0;
    state->current_story_day = -1;
    state->field_bg_cache_bank = -1;
    state->pending_sample_bank = -1;
    state->pending_stream_banks[1] = 0;
    state->pending_stream_banks[0] = 0;
    if (!(state->flags & 0x40)) {
        state->scene_audio.tracks.banks[0] = -1;
        state->scene_audio.tracks.keys[0][1] = -1;
        state->scene_audio.tracks.keys[0][0] = -1;
    }
    if (!(state->flags & 0x80)) {
        state->scene_audio.tracks.banks[1] = -1;
        state->scene_audio.tracks.keys[1][1] = -1;
        state->scene_audio.tracks.keys[1][0] = -1;
    }
    state->flags &= ~1;
    return 0;
}
