/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

typedef unsigned int u32;
typedef unsigned char u8;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideVoicePitchMasked(int *arg0) {
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
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x44) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ((u8 *)arg0)[0x10];
                    denom = *(int *)(voice - 0x34);
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x70;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x48) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = ((u8 *)arg0)[0x10];
                    denom = *(int *)(voice - 0x34);
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(short *)voice = step;
                    *(int *)(voice - 0x30) = (short)delta;
                }
            }
            i++;
            voice += sizeof(AkaoTrack);
            mask <<= 1;
        } while (i < 12);
    }
}
