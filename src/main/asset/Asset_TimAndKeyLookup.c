/* Contiguous scene-container helpers: TIM upload, key encoding and lookup. */
#include "common.h"
#include "pe1/asset_key_record.h"
#include "pe1/psyq_tim.h"
#include "pe1/scene_assets.h"

typedef unsigned char u8_1;

extern signed char g_Base32CharTable[];

int Gpu_LoadTimAsset(TimUploadRecord *asset, void *base) {
    RECT rect;
    int mask;
    u8 *saved_base;
    unsigned int packed;
    int h;
    unsigned int offset;
    int secondary;
    unsigned int image_offset;
    u8 *image_address;

    packed = asset->words[2];
    rect.x = (packed >> 10) & 0x7FF;
    packed = asset->words[2];
    rect.y = packed >> 21;
    packed = asset->words[2];
    rect.w = packed & 0x3FF;
    saved_base = base;
    packed = ((u8 *)asset)[7];
    if (packed != 0) {
        h = packed & 0xFF;
    } else {
        h = 0x100;
    }

    mask = 0xFFFFFF;
    rect.h = h;
    LoadImage(&rect, saved_base + (asset->words[1] & mask));

    secondary = asset->words[3] & mask;
    if (secondary != 0) {
        packed = asset->words[4];
        rect.x = (packed >> 10) & 0x7FF;
        packed = asset->words[4];
        rect.y = packed >> 21;
        packed = asset->words[4];
        rect.w = packed & 0x3FF;
        rect.h = ((u8 *)asset)[0xF];
        image_offset = asset->words[1] & mask;
        image_address = saved_base + image_offset;
        offset = asset->words[3] & mask;
        LoadImage(&rect, offset + image_address);
    }

    return 0;
}

int Str_EncodeBase32(char *out, unsigned int value) {
    int i;
    int last;
    char *dst;

    i = 0;
    last = 5;
    dst = out;
    while (i < 6) {
        int shift = ((last - i) * 5) + 2;
        u8_1 c = ((u8_1 *)g_Base32CharTable)[(value >> shift) & 0x1F];

        *dst = c;
        if ((unsigned int)(c - 0x61) < 0x1A) {
            *dst = c - 0x20;
        }
        i++;
        dst++;
    }

    out[6] = 0;
    return 0;
}


int Str_DecodeBase32(unsigned int a0) {
    int total = 0;
    int i;

    for (i = 1; i < 5; i++) {
        int factor = 1;
        int j;

        for (j = 0; j < 4 - i; j++) {
            factor *= 10;
        }

        total += (g_Base32CharTable[(a0 >> ((5 * (5 - i)) + 2)) & 0x1F] - 0x30) * factor;
    }

    return total;
}

int Str_ParseBase32Id(signed char *arg0) {
    unsigned int result = 0;
    int index = 0;

    while (index < 6) {
        signed char c = *arg0;
        int value = ((unsigned int)(c - '0') < 10U)
            ? (c - '0')
            : (c - 'W') - (c >= 'k') - (c >= 'r') - (c >= 'y');

        result |= (unsigned int)value << ((5 - index) * 5 + 2);
        index++;
        arg0++;
    }

    return result;
}

int Str_ParseMapNumber(signed char *arg0) {
    int hundreds;
    int tens;
    int result;

    hundreds = arg0[2] - 0x30;
    result = hundreds * 3;
    result <<= 3;
    result += hundreds;
    result <<= 2;

    tens = arg0[3] - 0x30;
    result += tens * 10;
    result -= 0x30;

    return result + arg0[4];
}

void *Asset_FindTable08ByU32Key(void *arg0, s32 arg1) {
    u32 mask;
    register u8 *table asm("$2");
    u32 descriptor;
    register AssetU32KeyRecord *record asm("$6");
    register s32 none asm("$7");
    register u32 i asm("$8");
    u32 data_mask;
    char frame[1];

    asm volatile("" : : "r"(frame));

    mask = 0x3FFFFF;
    descriptor = ((SceneAssetDirectory *)SceneAsset_ResolveOffset(arg0,
        ((SceneAssetBlob *)arg0)->directoryOffset))->keyEntries;
    i = 0;
    none = 0;
    table = (u8 *)arg0 + (descriptor & mask);
    descriptor >>= 22;

    if (descriptor != 0) {
        data_mask = 0xFFFFFF;
        record = (AssetU32KeyRecord *)table;
        do {
            if (record->key == arg1) {
                return (u8 *)arg0 + (record->packedOffset & data_mask);
            }
            i++;
            if (i < descriptor) {
                record++;
                continue;
            }
            break;
        } while (1);
    }

    return (void *)none;
}

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
