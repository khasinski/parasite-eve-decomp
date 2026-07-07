/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern char D_800BC078[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SlideAllVoicePan(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice asm("$6");
    int step;
    register int delta asm("$3");
    int denom;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = D_800BC078;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0x4C) & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = ((u8 *)arg0)[8];
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
