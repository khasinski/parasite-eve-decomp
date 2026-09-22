#ifndef PE1_SCENE_ASSETS_H
#define PE1_SCENE_ASSETS_H

#include "common.h"
#include "pe1/game_state.h"

/* Only the callback used during scene unloading is identified here. */
typedef struct SceneAssetHandler {
    u8 reserved[0x18];
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
} SceneAssetDirectory;

typedef struct SceneAssetBlob {
    unsigned int reserved;
    unsigned int directoryOffset;
} SceneAssetBlob;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneAssetHandler, unload) == 0x18,
                  scene_asset_unload_offset);
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
