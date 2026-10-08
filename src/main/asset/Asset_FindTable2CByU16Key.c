#include "common.h"
#include "pe1/scene_assets.h"

#define DEFINE_ASSET_SEARCHER(name, table_member, record_type) \
void *name(char *base, int key) { \
    u32 mask; \
    register char *table asm("$2"); \
    u32 descriptor; \
    register record_type *record asm("$6"); \
    register void *none asm("$7"); \
    register int i asm("$8"); \
    u32 data_mask; \
    char frame[1]; \
 \
    asm volatile("" : : "r"(frame)); \
    mask = 0x3FFFFF; \
    descriptor = ((SceneAssetDirectory *)(base + \
        ((SceneAssetBlob *)base)->directoryOffset))->table_member; \
    i = 0; \
    none = 0; \
    table = base + (descriptor & mask); \
    descriptor >>= 22; \
    if (descriptor != 0) { \
        data_mask = 0xFFFFFF; \
        record = (record_type *)table; \
        do { \
            if (record->key == key) { \
                return base + (record->offset & data_mask); \
            } \
            i++; \
            if (i < descriptor) { \
                record++; \
                continue; \
            } \
            break; \
        } while (1); \
    } \
    return none; \
}

DEFINE_ASSET_SEARCHER(Asset_FindTable2CByU16Key, streamEntries, SceneCdStreamRecord)
DEFINE_ASSET_SEARCHER(Asset_FindTable30ByU16Key, trackEntries, SceneTrackRecord)
