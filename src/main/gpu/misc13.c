#include "pe1/psyq_gpu.h"

typedef unsigned char u8;

int LoadImage(RECT *rect, int image);

typedef unsigned char u8_1;

extern u8_1 g_Base32CharTable[];

int Gpu_LoadTimAsset(unsigned int *asset, int base) {
    RECT rect;
    int mask;
    int saved_base;
    register unsigned int packed asm("$2");
    int h;
    register int offset asm("$5");
    int secondary;

    packed = asset[2];
    rect.x = (packed >> 10) & 0x7FF;
    packed = asset[2];
    rect.y = packed >> 21;
    packed = asset[2];
    rect.w = packed & 0x3FF;
    saved_base = base;
    packed = ((u8 *)asset)[7];
    if (packed != 0) {
        asm volatile("andi %0,%1,0xff" : "=r"(h) : "r"(packed));
    } else {
        h = 0x100;
    }

    mask = 0xFFFFFF;
    rect.h = h;
    offset = asset[1] & mask;
    LoadImage(&rect, saved_base + offset);

    secondary = asset[3] & mask;
    if (secondary != 0) {
        packed = asset[4];
        rect.x = (packed >> 10) & 0x7FF;
        packed = asset[4];
        rect.y = packed >> 21;
        packed = asset[4];
        rect.w = packed & 0x3FF;
        rect.h = ((u8 *)asset)[0xF];
        packed = asset[1] & mask;
        packed = saved_base + packed;
        offset = asset[3] & mask;
        LoadImage(&rect, packed + offset);
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
