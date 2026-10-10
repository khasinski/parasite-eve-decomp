#include "common.h"
#include "pe1/game_state.h"
#include "pe1/cdrom.h"
#include "pe1/asset_tim.h"
s32 DrawSync(s32 arg0);
extern s8 D_800B0CE0;
extern s8 g_LoadedTexturePageId;
extern u16 g_EntityTexLbaStartTbl[];
s32 Overlay_StreamTexturePage(void)
{
  Pe1GameState *state;
  register u16 *ranges;
  s32 retry;
  u16 *next_ranges;
  s32 offset;
  u16 start;
  s32 poll;
  state = &g_GameState;
  if (D_800B0CE0 != g_LoadedTexturePageId)
  {
    DrawSync(0);
    restart:
    ranges = g_EntityTexLbaStartTbl;

    next_ranges = ranges + 1;
    retry = -1;
    do
    {
      offset = ((s8)state->room_type + 0x2B) * 2;
      start = *((u16 *) (offset + ((s32) ranges)));
    }
    while (CdRom_ReadSectorsFromLba(state->pe_image_base_lba + start, state->scene_load_scratch, (*((u16 *) (offset + ((s32) next_ranges)))) - start) == retry);
    ;
    do
    {
      poll = CdRom_PollReady();
      if (poll == 0)
      {
        break;
      }
      if (poll == ((s32) ((u16 *) (-1))))
      {
        goto restart;
      }
    }
    while (1);
    Asset_LoadTimImage((TimFile *)state->scene_load_scratch);
    state->field_bg_cache_bank = (s8)state->room_type;
  }
  return 0;
}

/* Disc-change gating for scene-area transitions. */
extern unsigned char g_SceneAreaType;
extern unsigned char g_SavedSceneAreaType;
extern unsigned char g_DiscChangeFlags;

int CdRom_DetectDiscChange(void) {
    unsigned int *state = &g_GameState.flags;
    int offset = g_SceneAreaType - 0xA;

    if ((unsigned int)offset < 5) {
        int value = g_SavedSceneAreaType;

        if (g_SceneAreaType != (unsigned char)value) {
            *state |= 0x200000;
        }

        if (((unsigned int)offset >> 1) != ((value - 0xA) / 2)) {
            g_DiscChangeFlags |= 4;
        }
    }

    return 0;
}

#include "common.h"
#include "pe1/scene_transition.h"
#include "pe1/scene_entity_textures.h"
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "../../../tools/m2c/m2c_macros.h"
extern u8 D_800B0CE2[], D_8009D25C[];
extern u32 D_800B0DD8[];
/* Matching debt: pins, empty barriers, one retry jump, and a
 * 32-byte unused stack reserve retain retail allocation and scheduling.
 * The shifted state view preserves the bank-row address calculation. */
int Scene_LoadEntityTexture(void)
{
    int bank = *D_800B0CE2;
    u32 lba = *D_800B0DD8;
    Pe1GameState *state = &g_GameState;
    SceneAssetBlob *blob;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    TimUploadRecord *firstTim;
    u32 timOffset;
    SceneBankAssetRecord *record;
    SceneBankAssetRecord *firstRecord;
    register unsigned i asm("$16");
    u32 packed;
    u32 offsetMask;
    int index;
    int result;
    u32 matchingStackReserve[8];
retry:
    switch (state->entity_texture_phase) {
    case 0:
        if (state->flags & 0x200000)
            state->entity_texture_phase = 1;
        else
            state->entity_texture_phase = 6;
        break;
    case 1:
        index = bank + 3;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                state->scene_load_scratch,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 2;
        return 1;
    case 2:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 1;
            return 1;
        }
        if (result != 0)
            return 1;
        if (state->flags & 0x20000) {
            if (!(state->flags & 0x80000) && *D_8009D25C < 2)
                return 1;
            state->flags |= 0x40000;
        }
        state->entity_texture_phase = 3;
        break;
    case 3:
        blob = state->scene_load_scratch;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        timOffset = directory->timEntries & 0x3fffff;
        firstTim = SceneAsset_ResolveOffset(blob, timOffset);
        i = 0;
        if (directory->timEntries >> 22) {
            tim = firstTim;
            do {
                Gpu_LoadTimAsset(tim, blob);
                packed = directory->timEntries;
                ++i;
                ++tim;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 4;
        break;
    case 4:
        index = bank + 8;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                (void *)state->voice_bank_base_1400,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 5;
        return 1;
    case 5:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 4;
            return 1;
        }
        if (result != 0)
            return 1;
        state->entity_texture_phase = 6;
        break;
    case 6:
        blob = (SceneAssetBlob *)state->voice_bank_base_1400;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        {
            SceneBankAssetRecord *root = SceneAsset_ResolveOffset(blob, directory->bankRootEntries & 0x3fffff);
            state->bank_slots[0] = SceneAsset_ResolveOffset(blob, root->source.offsetAndId & 0xffffff);
        }
        firstRecord = SceneAsset_ResolveOffset(blob, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            offsetMask = 0xffffff;
            record = firstRecord;
            do {
                unsigned id;
                Pe1GameState *destination;
                asm("" : "=r"(record) : "0"(record), "r"(offsetMask));
                id = record->source.bytes.id;
                destination = (Pe1GameState *)(id * 4 + (u32)state);
                destination->bank_rows[0][0] = SceneAsset_ResolveOffset(
                    blob, record->source.offsetAndId & offsetMask);
                packed = directory->bankRowEntries;
                asm volatile("" : "=r"(packed) : "0"(packed));
                ++i;
                ++record;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 0;
        state->loaded_entity_bank = state->requested_entity_bank;
        state->flags &= ~0x200000;
        return 0;
    default:
        return 0;
    }
    goto retry;
}

void Entity_SetActionMode(void *entity, int mode);
/* Keep independent C lvalues for the retail flag loads across switch cases. */
extern struct { char _[16]; } D_800B0CE2_o __asm__("g_SceneAreaType");
extern struct { char _[16]; } D_800B0CE2_w __asm__("g_SceneAreaType");
#define g_SceneAreaType (*(u8 *)&D_800B0CE2_o)
extern struct { char _[16]; } D_800B0CE3_o __asm__("g_SavedSceneAreaType");
#define g_SavedSceneAreaType (*(u8 *)&D_800B0CE3_o)
extern struct { char _[16]; } D_800B0CE4_o __asm__("D_800B0CE4");
#define g_CurrentStoryDay (*(s8 *)&D_800B0CE4_o)
extern struct { char _[16]; } D_800B0CE6_o __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_w __asm__("g_DiscChangeFlags");
extern Pe1GameState g_GameStateFlagsAfterPlayerInit __asm__("g_GameState");
#define g_DiscChangeFlags (*(u8 *)&D_800B0CE6_o)
extern struct { char _[16]; } D_800B0CEB_o __asm__("g_SceneAreaTypeDiscSwapBackup");
#define g_SceneAreaTypeDiscSwapBackup (*(u8 *)&D_800B0CEB_o)
extern struct { char _[16]; } D_800B0DC5_o __asm__("D_800B0DC5");
#define D_800B0DC5 (*(u8 *)&D_800B0DC5_o)
extern struct { char _[16]; } D_800B0E70_o __asm__("g_SceneMapPrimBaseTable");
#define g_SceneMapPrimBaseTable (*(s32 *)&D_800B0E70_o)
extern struct { char _[16]; } D_800B0EEC_o __asm__("D_800B0EEC");
#define D_800B0EEC (*(s32 *)&D_800B0EEC_o)
extern struct { char _[16]; } D_800B89F8_o __asm__("g_EntityRenderScratch");
#define g_EntityRenderScratch (*(M2C_UNK *)&D_800B89F8_o)
extern struct { char _[16]; } D_800BEA40_o __asm__("D_800BEA40");
#define D_800BEA40 (*(M2C_UNK *)&D_800BEA40_o)

s32 Scene_InitEntityPlayer(s32 arg0) {
    Pe1GameState *gameState;
    register s32 arg0v asm("$17");
    M2C_UNK sp28;
    s32 var_v0_2;
    u32 temp_a1;
    u8 temp_v1;
    u8 tce6;
    u8 tce3;
    FieldActor *playerForActionMode;
    FieldActor *playerForRenderSetup;
    FieldActor *playerForRoomPrims;
    FieldActor *playerForAnimation;
    FieldActor *playerForScaleUpdate;
    u32 th1;
    register s32 phaseValue asm("$2");

    arg0v = arg0;
    gameState = &g_GameState;
    switch (D_800B0DC5) {
    case 32:
        g_GameState.flags = g_GameState.flags | 0x20000;
        if (arg0v != 0) {
            temp_v1 = g_SceneAreaType;
            phaseValue = 0xE;
            (*(u8 *)&D_800B0CE2_w) = phaseValue;
            g_SceneAreaTypeDiscSwapBackup = temp_v1;
            phaseValue = 0x21;
            goto block_20;
        } else {
            phaseValue = g_SceneAreaTypeDiscSwapBackup;
            g_SceneAreaType = phaseValue;
            phaseValue = 0x21;
            goto block_20;
        }
    case 33:
        phaseValue = 0x22;
        goto block_20;
    case 34:
        phaseValue = 0x23;
        goto block_20;
    case 35:
        temp_a1 = g_SceneAreaType - 0xA;
        if (temp_a1 < 5U) {
            tce3 = g_SavedSceneAreaType;
            if (g_SceneAreaType != tce3) {
                g_GameStateFlagsAfterPlayerInit.flags = (g_GameState.flags | 0x200000);
            }
            th1 = temp_a1 >> 1;
            if (th1 != ((tce3 - 0xA) / 2)) {
                tce6 = g_DiscChangeFlags;
                (*(u8 *)&D_800B0CE6_w) = tce6 | 4;
            }
        }
                phaseValue = 0x24;
        goto block_20;
    case 36:
        if (Scene_LoadEntityTexture() != 1) {
            if (arg0v == 0) {
                phaseValue = 0x25;
                goto block_20;
            }
            goto block_19;
        }
        goto block_21;
    case 37:
        g_DiscChangeFlags |= 4;
        Scene_SetStoryDay(g_CurrentStoryDay);
        gameState->scene_init_phase = 0x26;
        return 1;
    case 38:
        phaseValue = 0x27;
        if (Scene_LoadEntityTextures() == 1) {
            if ((u8) gameState->scene_init_subphase >= 0xBU) {
                goto block_19;
            }
            goto block_21;
        } else {
            phaseValue = 0x27;
            goto block_20;
        }
block_19:
        phaseValue = 0x27;
block_20:
        gameState->scene_init_phase = phaseValue;
block_21:
        return 1;
    case 39:
        playerForActionMode = g_PlayerEntity;
        M2C_FIELD(playerForActionMode, s32 *, 0x1AC) = g_SceneMapPrimBaseTable;
        playerForActionMode->action_data = (void *)D_800B0EEC;
        Entity_SetActionMode(playerForActionMode, 0x15);
        playerForRenderSetup = g_PlayerEntity;
        Render_SetupEntityPrims(&playerForRenderSetup->render_object, playerForRenderSetup->model_state.model_header, (u8 *)(playerForRenderSetup->allocation_block + 0x50), 0x3C0, 0x100, 0, 0x1C0, 2, (s8 **)&sp28, 1);
        playerForRoomPrims = g_PlayerEntity;
        Render_InitRoomPrimState(&playerForRoomPrims->render_object);
        playerForAnimation = g_PlayerEntity;
        Render_DrawWithAnim(&playerForAnimation->render_object, (s32)playerForAnimation->action_data, 0, &D_800BEA40, &g_EntityRenderScratch);
        playerForScaleUpdate = g_PlayerEntity;
        playerForScaleUpdate->render_object.header->shadow_radius = (s16) (playerForScaleUpdate->render_object.hit_cylinder.radius * 2);
        g_GameStateFlagsAfterPlayerInit.flags = (g_GameState.flags & 0xFFF9FFFF);
        if (arg0v != 0) {
            phaseValue = gameState->flags;
            var_v0_2 = phaseValue | 0x80000;
        } else {
            var_v0_2 = gameState->flags & 0xFFF7FFFF;
        }
        gameState->flags = var_v0_2;
        gameState->scene_init_phase = 0x20;
        /* fallthrough */
    default:
        return 0;
    }
}

/* Independent global views below use the shared typed declarations. */
#undef g_SceneAreaType
#undef g_SavedSceneAreaType
#undef g_CurrentStoryDay
#undef g_DiscChangeFlags
#undef g_SceneAreaTypeDiscSwapBackup
#undef D_800B0DC5
#undef g_SceneMapPrimBaseTable
#undef D_800B0EEC
#undef g_EntityRenderScratch
#undef D_800BEA40


extern u32 D_8009D2E8;
/* Existing byte symbols for the shared state's pending/current day and flags.
 * Preserve the unsigned pending-byte read before its signed-day conversion. */
extern volatile u8 D_800B0CE5;
extern u8 D_800B0CE6;

int Scene_SetStoryDay(s32 storyDay) {
    Pe1GameState *gameState = &g_GameState;
    u8 flags;

    if (storyDay == -1) {
        u8 pending;
        u8 storyFlags;
        pending = D_800B0CE5;
        storyFlags = D_800B0CE6;
        storyDay = (s8)pending;
        D_800B0CE4 = pending;
        D_800B0CE6 = storyFlags | 3;
    }

    if (((g_GameStateFlags & 2) != 0) || ((gameState->flags & 2) != 0)) {
        D_800B0CE6 |= 2;
        D_8009D2E8 &= ~2U;
    }

    flags = gameState->story_day_flags;
    if ((flags & 4) != 0) {
        gameState->story_day_flags = (flags | 3) & ~4;
    }

    if ((storyDay - 1U) < 8U) {
        if (storyDay != (s8)gameState->pending_story_day) {
            s8 newStoryDay;

            newStoryDay = storyDay;
            gameState->pending_story_day = newStoryDay;
            gameState->current_story_day = newStoryDay;
            gameState->story_day_flags |= 1;
        }
    }

    return 0;
}


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
    RenderObjectHeader *model;
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
        streamEntries = directory->streamEntries;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->streamEntries >> 22);
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
        streamEntries = directory->streamEntries;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->streamEntries >> 22);
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
            state->scene_object_work, 0x2C0, 0x80, 0, 0x1C2, 0, (s8 **)setup, 1);
        if (state->scene_object.animation_source == 0 && g_PlayerEntity != 0) {
            state->scene_object.animation_source = &g_PlayerEntity->render_object;
            state->scene_object.animation_state = 3;
            state->scene_object.animation_id = 0x12;
        }
        state->scene_object.shade = 0x80;
        state->scene_object.lightNegativeY = 0xC;
        state->scene_object.lightPositiveY = 0x18;
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
