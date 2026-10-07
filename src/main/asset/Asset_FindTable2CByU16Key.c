#include "common.h"
#include "pe1/asset_key_record.h"

#define DEFINE_ASSET_SEARCHER(name, table_offset) \
void *name(char *base, int key) { \
    u32 mask; \
    register char *table asm("$2"); \
    u32 descriptor; \
    register AssetU16KeyRecord *record asm("$6"); \
    register void *none asm("$7"); \
    register int i asm("$8"); \
    u32 data_mask; \
    char frame[1]; \
 \
    asm volatile("" : : "r"(frame)); \
    mask = 0x3FFFFF; \
    descriptor = *(u32 *)(base + *(u32 *)(base + 4) + (table_offset)); \
    i = 0; \
    none = 0; \
    table = base + (descriptor & mask); \
    descriptor >>= 22; \
    if (descriptor != 0) { \
        data_mask = 0xFFFFFF; \
        record = (AssetU16KeyRecord *)table; \
        do { \
            if (record->key == key) { \
                return base + (record->packedOffset & data_mask); \
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

DEFINE_ASSET_SEARCHER(Asset_FindTable2CByU16Key, 0x2C)
DEFINE_ASSET_SEARCHER(Asset_FindTable30ByU16Key, 0x30)
