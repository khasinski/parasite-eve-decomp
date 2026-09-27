#ifndef PE1_SCENE_ASSETS_H
#define PE1_SCENE_ASSETS_H

#include "common.h"
#include "pe1/game_state.h"

/* Scene startup and teardown callbacks; other handlers remain unidentified. */
typedef struct SceneAssetHandler {
    void (*init)(void);
    u8 reserved[0x14];
    void (*unload)(void);
} SceneAssetHandler;

typedef struct SceneAssetRecord {
    u8 reserved[7];
    u8 handlerId;
    u8 trailing[4];
} SceneAssetRecord;

typedef struct SceneAssetDirectory {
    unsigned int reserved;
    /* Low 22 bits: byte offset from the blob; high 10 bits: record count. */
    unsigned int entries;
    unsigned int reserved08;
    unsigned int bankRootEntries; /* 0x0C */
    unsigned int bankRowEntries;  /* 0x10 */
    unsigned int reserved14[5];
    unsigned int timEntries; /* 0x28: same offset/count encoding */
    unsigned int reserved2c;
    unsigned int trackEntries; /* 0x30: same offset/count encoding */
} SceneAssetDirectory;

typedef struct SceneTrackRecord {
    u32 size;   /* Low 24 bits: bytes copied to the selected bank workspace. */
    u32 offset; /* Low 24 bits: source offset in the loaded scene blob. */
    u16 bank;
    u16 key;
} SceneTrackRecord;

/* Sector offsets follow the archive's absolute base LBA in variable-size data. */
typedef struct SceneSectorDirectory {
    u32 base_lba;
    u16 offsets[0];
} SceneSectorDirectory;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneSectorDirectory, offsets) == 4,
                  scene_sector_offsets_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, cd_read_phase) == 0xF0,
                  scene_cd_read_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, pe_image_base_lba) == 0x100,
                  scene_cd_image_base_offset);

typedef struct SceneBankAssetRecord {
    u32 reserved;
    union {
        u32 offsetAndId;
        struct { u8 offset[3]; u8 id; } bytes;
    } source;
    u32 trailing;
} SceneBankAssetRecord;

PE1_STATIC_ASSERT(sizeof(SceneBankAssetRecord) == 12, scene_bank_asset_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneBankAssetRecord, source.bytes.id) == 7,
                  scene_bank_asset_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetDirectory, bankRootEntries) == 0xC,
                  scene_bank_root_entries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetDirectory, bankRowEntries) == 0x10,
                  scene_bank_row_entries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, requested_entity_bank) == 0xA,
                  scene_requested_entity_bank_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, loaded_entity_bank) == 0xB,
                  scene_loaded_entity_bank_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, entity_texture_phase) == 0xEC,
                  scene_entity_texture_phase_offset);

PE1_STATIC_ASSERT(sizeof(SceneTrackRecord) == 12, scene_track_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneTrackRecord, bank) == 8, scene_track_bank_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneTrackRecord, key) == 10, scene_track_key_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetDirectory, trackEntries) == 0x30,
                  scene_track_entries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.banks) == 0xDA,
                  scene_track_banks_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.keys) == 0xDC,
                  scene_track_keys_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, cd_track_phase) == 0xF1,
                  scene_track_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, scene_audio.tracks.pending_key) == 0xE0,
                  scene_pending_track_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, pending_sample_bank) == 0xE8,
                  scene_pending_sample_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, pending_stream_banks) == 0xEA,
                  scene_pending_streams_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, cd_transition_phase) == 0xF2,
                  scene_transition_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, transition_volume) == 0xFE,
                  scene_transition_volume_offset);

typedef struct SceneAssetBlob {
    unsigned int reserved;
    unsigned int directoryOffset;
} SceneAssetBlob;

/* Scene containers encode byte offsets relative to their loaded base. */
static inline void *SceneAsset_ResolveOffset(void *base, unsigned int offset)
{
    return (u8 *)base + offset;
}

typedef struct TimUploadRecord { u32 words[5]; } TimUploadRecord;
typedef struct TimPackedImage {
    u32 key;
    union {
        u32 offsetAndHeight;
        struct { u8 offset[3]; u8 height; } bytes;
    } source;
    u32 geometry;
} TimPackedImage;

PE1_STATIC_ASSERT(sizeof(TimUploadRecord) == 20, scene_tim_upload_record_size);
PE1_STATIC_ASSERT(sizeof(TimPackedImage) == 12, scene_tim_packed_image_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TimPackedImage, source.bytes.height) == 7,
                  scene_tim_image_height_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TimPackedImage, geometry) == 8,
                  scene_tim_image_geometry_offset);

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetHandler, unload) == 0x18,
                  scene_asset_unload_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetHandler, init) == 0,
                  scene_asset_init_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetDirectory, timEntries) == 0x28,
                  scene_asset_tim_entries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, tim_load_state) == 0xEF,
                  game_state_tim_load_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, pe_image_base_lba) == 0x100,
                  game_state_pe_image_base_lba_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, scene_process_slots) == 0x188,
                  game_state_scene_process_slots_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, scene_load_scratch) == 0x194,
                  game_state_scene_load_scratch_offset);
PE1_STATIC_ASSERT(sizeof(SceneAssetRecord) == 12, scene_asset_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetRecord, handlerId) == 7,
                  scene_asset_handler_id_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetDirectory, entries) == 4,
                  scene_asset_directory_entries_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetBlob, directoryOffset) == 4,
                  scene_asset_directory_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, loaded_scene_assets) == 0x18C,
                  game_state_scene_assets_offset);

extern unsigned int g_GameStateFlags;
/* Opaque handler pointers; the unload path reads the prefix declared above. */
extern void **g_PmCmdHandlerTable;
extern void *D_800E1044[104];

int Asset_UnloadTableEntries(void);
int Asset_LoadTimTextures(int force);
int Gpu_LoadTimAsset(TimUploadRecord *record, void *base);
extern u16 D_800930E2, D_800930E4;

/* The initializer clears each record's halfwords at offsets 6 and 4. */
typedef struct SceneBankResetPair {
    u32 reserved;
    u16 first, second;
} SceneBankResetPair;
extern SceneBankResetPair D_80094488[4];
int Asset_FindTable08ByU32Key(void *base, s32 key);
void Akao_LoadVoiceBankAlt(void);
void Akao_ClearVoiceBank(void);

PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, bank_asset_table) == 0x124,
                  game_state_bank_asset_table_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, bank_asset_source) == 0x14C,
                  game_state_bank_asset_source_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, bank_slots) == 0x198,
                  game_state_bank_slots_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, bank_rows) == 0x1C0,
                  game_state_bank_rows_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, bank_reset_940) == 0x940,
                  game_state_bank_reset_offset);
PE1_STATIC_ASSERT(sizeof(Pe1GameState) == 0x95C, game_state_size);
PE1_STATIC_ASSERT(sizeof(SceneBankResetPair) == 8, scene_bank_reset_pair_size);

#endif
