#include "common.h"
#include "pe1/scene_entity_textures.h"

/* Streams the area entity bank and rebuilds the scene render object. */
int Scene_LoadEntityTextures(void)
{
    int area = g_SceneAreaType;
    int day = D_800B0CE4;
    u32 lba = g_PeImageBaseLba;
    Pe1GameState *state = &g_GameState;
    SceneAssetView *view;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    TimUploadRecord *firstTim;
    SceneBankAssetRecord *firstRecord;
    SceneBankAssetRecord *root;
    void *model;
    u32 streamEntries, packed;
    u32 i;
    int result;
    /* Render_SetupEntityPrims output; word 0 doubles as the sector index. */
    int setup[16];

    if (area - 10U >= 5)
        return 0;
    /*
     * Recorded crutch debt: retail's state re-dispatch is a goto restart
     * loop, not reproducible with a structured loop in stock GCC 2.7.2
     * (loop depth would reweight register priorities).
     */
retry:
    switch (state->scene_init_subphase) {
    case 0:
        if (state->story_day_flags & 1) {
            state->scene_init_subphase = 1;
            break;
        }
        if (state->story_day_flags & 2) {
            state->scene_init_subphase = 11;
            break;
        }
        Scene_UpdateBgDraw();
        return 0;
    case 1:
        if (day - 2U >= 5) {
            state->scene_init_subphase = 4;
            break;
        }
        setup[0] = day + 0x26;
        result = CdRom_ReadSectorsFromLba(lba + D_800930D8[setup[0]],
            state->scene_load_scratch,
            D_800930DA[setup[0]] - D_800930D8[setup[0]]);
        Scene_UpdateBgDraw();
        if (result != -1)
            state->scene_init_subphase = 2;
        return 1;
    case 2:
        result = CdRom_PollReady();
        if (result == -1) {
            state->scene_init_subphase = 1;
            return 1;
        }
        if (result != 0) {
            if (!(g_GameStateFlags & 2))
                Scene_UpdateBgDraw();
            return 1;
        }
        state->scene_init_subphase = 3;
        break;
    case 3:
        view = state->scene_load_scratch;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstTim = SCENE_ASSET_AT(view, directory->timEntries & 0x3fffff);
        i = 0;
        if (directory->timEntries >> 22) {
            tim = firstTim;
            do {
                Gpu_LoadTimAsset(tim, view);
                packed = directory->timEntries;
                i++;
                tim++;
            } while (i < packed >> 22);
        }
        state->scene_init_subphase = 4;
        break;
    case 4:
        setup[0] = (state->requested_entity_bank - 10) / 2 * 8 + day + 0x16;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[setup[0]],
                state->entity_texture_blob,
                D_800930DA[setup[0]] - D_800930D8[setup[0]]) != -1)
            state->scene_init_subphase = 5;
        if (!(g_GameStateFlags & 2))
            Scene_UpdateBgDraw();
        return 1;
    case 5:
        result = CdRom_PollReady();
        if (result == -1) {
            state->scene_init_subphase = 4;
            return 1;
        }
        if (result != 0) {
            if (!(g_GameStateFlags & 2))
                Scene_UpdateBgDraw();
            return 1;
        }
        state->scene_init_subphase = 6;
        break;
    case 6:
        view = state->entity_texture_blob;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstRecord = SCENE_ASSET_AT(view, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            do {
                state->bank_rows[0][firstRecord[i].source.bytes.id] = SCENE_ASSET_AT(view, firstRecord[i].source.offsetAndId & 0xffffff);
                packed = directory->bankRowEntries;
                i++;
            } while (i < packed >> 22);
        }
        state->cd_range_read_mode = 0;
        for (i = 0; i < 3; i++)
            state->scene_object_tables[i] = 0;
        streamEntries = directory->reserved2c;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->reserved2c >> 22);
            }
            state->cd_range_read_mode = first->cdIndex;
        }
        if (g_GameStateFlags & 2) {
            state->scene_init_subphase = 7;
            return 1;
        }
        if (state->story_day_flags & 2) {
            state->scene_init_subphase = 11;
            break;
        }
        Scene_UpdateBgDraw();
        state->scene_init_subphase = 0;
        state->story_day_flags &= ~3;
        return 0;
    case 7:
        if (CD_ReadSectors(1, state->cd_range_read_mode, 0,
                state->scene_load_scratch, 0x21, 0) == 0) {
            state->scene_init_subphase = 11;
            break;
        }
        return 1;
    case 11:
        state->story_day_flags |= 1;
        state->scene_init_subphase = 12;
        return 1;
    case 12:
        state->scene_init_subphase = 13;
        return 1;
    case 13:
        view = state->entity_texture_blob;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstRecord = SCENE_ASSET_AT(view, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            do {
                state->bank_rows[0][firstRecord[i].source.bytes.id] = SCENE_ASSET_AT(view, firstRecord[i].source.offsetAndId & 0xffffff);
                packed = directory->bankRowEntries;
                i++;
            } while (i < packed >> 22);
        }
        state->cd_range_read_mode = 0;
        for (i = 0; i < 3; i++)
            state->scene_object_tables[i] = 0;
        streamEntries = directory->reserved2c;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->reserved2c >> 22);
            }
            state->cd_range_read_mode = first->cdIndex;
        }
        if (g_PlayerEntity == 0) {
            u32 sceneFlag = state->flags & 2;
            return sceneFlag == 0;
        }
        if (g_GameStateFlags & 2) {
            root = SCENE_ASSET_AT(view, directory->bankRootEntries & 0x3fffff);
            model = SCENE_ASSET_AT(view, root->source.offsetAndId & 0xffffff);
        } else {
            model = state->scene_object_model;
        }
        Render_SetupEntityPrims(&state->scene_object, model,
            state->scene_object_work, 0x2C0, 0x80, 0, 0x1C2, 0, setup, 1);
        if (state->scene_object.animation_source == 0 && g_PlayerEntity != 0) {
            state->scene_object.animation_source = &g_PlayerEntity->render_object;
            state->scene_object.animation_state = 3;
            state->scene_object.animation_id = 0x12;
        }
        state->scene_object.shade = 0x80;
        state->scene_object.light_negative_y = 0xC;
        state->scene_object.light_positive_y = 0x18;
        Render_InitRoomPrimState(&state->scene_object);
        Render_DrawWithAnim(&state->scene_object, 0, 0, D_800BEA40,
            g_EntityRenderScratch);
        state->scene_init_subphase = 0;
        state->story_day_flags &= ~3;
        return 1;
    default:
        return 0;
    }
    goto retry;
}
