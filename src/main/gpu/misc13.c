#include "common.h"
#include "pe1/psyq_tim.h"
#include "pe1/scene_assets.h"

typedef unsigned char u8_1;

extern u8_1 g_Base32CharTable[];

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
        u8_1 c = g_Base32CharTable[(value >> shift) & 0x1F];

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
