/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

typedef unsigned int u32;
typedef unsigned short u16;

extern char D_800BC074[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoiceVolume(int *arg0) {
    u32 mask;
    u32 active;
    u32 block_flag;
    u32 i;
    register char *voice asm("$6");
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC074;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x48) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = (short)(((arg0[2] & 0x7F) << 8) - *(u16 *)(voice + 0x64));
                *(u16 *)voice = step;
                *(u16 *)(voice + 0x66) = delta / (short)step;
            }
        }
        i++;
        voice += sizeof(AkaoTrack);
        mask <<= 1;
    } while (i < 12);
}
