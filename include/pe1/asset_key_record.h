#ifndef PE1_ASSET_KEY_RECORD_H
#define PE1_ASSET_KEY_RECORD_H

#include "common.h"

/* Word-keyed entries in SceneAssetDirectory.keyEntries. */
typedef struct AssetU32KeyRecord {
    u32 reserved00;
    u32 packedOffset;
    s32 key;
} AssetU32KeyRecord;

PE1_STATIC_ASSERT(sizeof(AssetU32KeyRecord) == 12, asset_u32_key_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(AssetU32KeyRecord, key) == 8, asset_u32_key_offset);

#endif
