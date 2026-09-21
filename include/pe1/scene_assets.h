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

#endif
