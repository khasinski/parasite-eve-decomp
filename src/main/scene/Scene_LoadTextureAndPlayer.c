#include "common.h"
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/field_actor.h"
#include "../../../tools/m2c/m2c_macros.h"
extern u8 D_800B0CE2[], D_8009D25C[];
extern u32 D_800B0DD8[];
extern u16 D_800930D8[], D_800930DA[];
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
M2C_UNK Render_SetupEntityPrims(void *, s32, s32, M2C_UNK, s32, s32, s32, s32, M2C_UNK *, s32); /* extern */
M2C_UNK Render_DrawWithAnim(void *, s32, M2C_UNK, M2C_UNK *, M2C_UNK *); /* extern */
int Render_InitRoomPrimState(void *object);
int Scene_LoadEntityTexture(void);
void Scene_SetStoryDay(int storyDay);
int Scene_LoadEntityTextures(void);
/* Local typed aliases retain the independent player-pointer load sites. */
extern FieldActor *g_PlayerEntityForActionMode __asm__("g_PlayerEntity");
extern FieldActor *g_PlayerEntityForRenderSetup __asm__("g_PlayerEntity");
extern FieldActor *g_PlayerEntityForRoomPrims __asm__("g_PlayerEntity");
extern FieldActor *g_PlayerEntityForAnimation __asm__("g_PlayerEntity");
extern FieldActor *g_PlayerEntityForScaleUpdate __asm__("g_PlayerEntity");
/* Keep independent C lvalues for the retail flag loads across switch cases. */
extern Pe1GameState g_GameStateFlagsCase32 __asm__("g_GameState");
extern struct { char _[16]; } D_800B0CE2_o __asm__("g_SceneAreaType");
extern struct { char _[16]; } D_800B0CE2_w __asm__("g_SceneAreaType");
#define g_SceneAreaType (*(u8 *)&D_800B0CE2_o)
extern struct { char _[16]; } D_800B0CE3_o __asm__("g_SavedSceneAreaType");
extern struct { char _[16]; } D_800B0CE3_s1 __asm__("g_SavedSceneAreaType");
#define g_SavedSceneAreaType (*(u8 *)&D_800B0CE3_o)
extern struct { char _[16]; } D_800B0CE4_o __asm__("D_800B0CE4");
#define g_CurrentStoryDay (*(s8 *)&D_800B0CE4_o)
extern struct { char _[16]; } D_800B0CE6_o __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_w __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_o2 __asm__("g_DiscChangeFlags");
extern struct { char _[16]; } D_800B0CE6_w2 __asm__("g_DiscChangeFlags");
extern Pe1GameState g_GameStateFlagsBeforeSceneSwitch __asm__("g_GameState");
extern Pe1GameState g_GameStateFlagsAfterSceneSwitch __asm__("g_GameState");
extern Pe1GameState g_GameStateFlagsBeforePlayerInit __asm__("g_GameState");
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
    register s32 tld2 asm("$2");
    register s8 var_v0 asm("$2");
    u32 temp_a1;
    u8 temp_v1;
    u8 tce6;
    u8 tce3;
    void *playerForActionMode;
    void *playerForRenderSetup;
    void *playerForRoomPrims;
    void *playerForAnimation;
    void *playerForScaleUpdate;
    u32 th1;
    register u8 k0e asm("$2");
    register u8 ttb asm("$2");

    arg0v = arg0;
    gameState = &g_GameState;
    switch (D_800B0DC5) {
    case 32:
        g_GameStateFlagsCase32.flags = g_GameStateFlagsCase32.flags | 0x20000;
        if (arg0v != 0) {
            temp_v1 = g_SceneAreaType;
            k0e = 0xE;
            (*(u8 *)&D_800B0CE2_w) = k0e;
            g_SceneAreaTypeDiscSwapBackup = temp_v1;
            var_v0 = 0x21;
            goto block_20;
        } else {
            ttb = g_SceneAreaTypeDiscSwapBackup;
            g_SceneAreaType = ttb;
            var_v0 = 0x21;
            goto block_20;
        }
    case 33:
        var_v0 = 0x22;
        goto block_20;
    case 34:
        var_v0 = 0x23;
        goto block_20;
    case 35:
        temp_a1 = g_SceneAreaType - 0xA;
        if (temp_a1 < 5U) {
            tce3 = g_SavedSceneAreaType;
            if (g_SceneAreaType != tce3) {
                g_GameStateFlagsAfterSceneSwitch.flags = (g_GameStateFlagsBeforeSceneSwitch.flags | 0x200000);
            }
            th1 = temp_a1 >> 1;
            if (th1 != ((tce3 - 0xA) / 2)) {
                tce6 = g_DiscChangeFlags;
                (*(u8 *)&D_800B0CE6_w) = tce6 | 4;
            }
        }
                var_v0 = 0x24;
        goto block_20;
    case 36:
        if (Scene_LoadEntityTexture() != 1) {
            if (arg0v == 0) {
                var_v0 = 0x25;
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
        var_v0 = 0x27;
        if (Scene_LoadEntityTextures() == 1) {
            if ((u8) gameState->scene_init_subphase >= 0xBU) {
                goto block_19;
            }
            goto block_21;
        } else {
            var_v0 = 0x27;
            goto block_20;
        }
block_19:
        var_v0 = 0x27;
block_20:
        gameState->scene_init_phase = var_v0;
block_21:
        return 1;
    case 39:
        playerForActionMode = g_PlayerEntityForActionMode;
        M2C_FIELD(playerForActionMode, s32 *, 0x1AC) = g_SceneMapPrimBaseTable;
        M2C_FIELD(playerForActionMode, s32 *, 0x1B0) = D_800B0EEC;
        Entity_SetActionMode(playerForActionMode, 0x15);
        playerForRenderSetup = g_PlayerEntityForRenderSetup;
        Render_SetupEntityPrims(playerForRenderSetup + 0x1B4, M2C_FIELD(playerForRenderSetup, s32 *, 0x1AC), M2C_FIELD(playerForRenderSetup, s32 *, 0x278) + 0x50, 0x3C0, 0x100, 0, 0x1C0, 2, &sp28, 1);
        playerForRoomPrims = g_PlayerEntityForRoomPrims;
        Render_InitRoomPrimState(playerForRoomPrims + 0x1B4);
        playerForAnimation = g_PlayerEntityForAnimation;
        Render_DrawWithAnim(playerForAnimation + 0x1B4, M2C_FIELD(playerForAnimation, s32 *, 0x1B0), 0, &D_800BEA40, &g_EntityRenderScratch);
        playerForScaleUpdate = g_PlayerEntityForScaleUpdate;
        M2C_FIELD(M2C_FIELD(playerForScaleUpdate, void **, 0x1B4), s16 *, 0x14) = (s16) (M2C_FIELD(playerForScaleUpdate, s16 *, 0x224) * 2);
        g_GameStateFlagsAfterPlayerInit.flags = (g_GameStateFlagsBeforePlayerInit.flags & 0xFFF9FFFF);
        if (arg0v != 0) {
            tld2 = M2C_FIELD(gameState, s32 *, 0);
            var_v0_2 = tld2 | 0x80000;
        } else {
            var_v0_2 = M2C_FIELD(gameState, s32 *, 0) & 0xFFF7FFFF;
        }
        M2C_FIELD(gameState, s32 *, 0) = var_v0_2;
        gameState->scene_init_phase = 0x20;
        /* fallthrough */
    default:
        return 0;
    }
}
