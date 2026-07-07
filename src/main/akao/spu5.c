/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetVoicePanImmediateMasked(int *arg0) {
    register char *base asm("$3");
    u32 active;
    u32 mask;
    u32 i;
    register char *voice asm("$5");
    int value;
    int dirty;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = ((u8 *)arg0)[0xC];
                    dirty = *(volatile u32 *)voice;
                    *(u16 *)(voice - 0x7C) = 0;
                    value <<= 8;
                    dirty |= AKAO_VOICE_PARAM_VOLUME;
                    *(u16 *)(voice - 0x7E) = value;
                    *(u32 *)voice = dirty;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}

void Spu_SlideVoicePanMasked(int *arg0) {
    register char *base asm("$3");
    u32 active;
    u32 mask;
    u32 i;
    register char *voice asm("$6");
    int step;
    register int delta asm("$3");
    int denom;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = AKAO_SPU_VOICE_SFX_START_MASK;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x4C) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ((u8 *)arg0)[0x10];
                    denom = *(u16 *)(voice - 2);
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(u16 *)voice = step;
                    *(u16 *)(voice + 0x64) = delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x78;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x50) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ((u8 *)arg0)[0x10];
                    denom = *(u16 *)(voice - 2);
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(u16 *)voice = step;
                    *(u16 *)(voice + 0x64) = delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}
