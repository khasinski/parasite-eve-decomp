#ifndef PE1_ASSET_KEY_RECORD_H
#define PE1_ASSET_KEY_RECORD_H

#include "common.h"

/* Asset descriptor tables use 12-byte records. The key occupies either
 * the final word or its upper halfword, depending on the table. */
typedef struct AssetU16KeyRecord {
    u32 reserved00;
    u32 packedOffset;
    u16 reserved08;
    u16 key;
} AssetU16KeyRecord;

typedef struct AssetU32KeyRecord {
    u32 reserved00;
    u32 packedOffset;
    s32 key;
} AssetU32KeyRecord;

PE1_STATIC_ASSERT(sizeof(AssetU16KeyRecord) == 12, asset_u16_key_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(AssetU16KeyRecord, key) == 10, asset_u16_key_offset);
PE1_STATIC_ASSERT(sizeof(AssetU32KeyRecord) == 12, asset_u32_key_record_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(AssetU32KeyRecord, key) == 8, asset_u32_key_offset);

#endif
