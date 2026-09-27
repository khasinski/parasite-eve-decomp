#include "common.h"
/* MASPSX_FLAGS: --expand-div */
#include "pe1/akao.h"

extern char D_800BC078[];
extern u32 g_SpuActiveVoiceMask;

static inline int PanDelta(int target, int current, int step) {
    return (short)((target << 8) - current) / (short)step;
}

void Spu_SlideAllVoicePan(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    AkaoTrack *voice;
    int step;
    int delta;

    mask = AKAO_SPU_VOICE_SFX_START_MASK;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = (AkaoTrack *)(D_800BC078 - 0x78);

    do {
        if ((active & mask) != 0) {
            if ((voice->key_on_mask & block_flag) == 0) {
                step = 1;
                if (arg0[1] != 0) {
                    step = arg0[1];
                }
                delta = PanDelta(((u8 *)arg0)[8], voice->panpot, step);
                voice->panpot_delta = delta;
                voice->panpot_slide_duration = step;
            }
        }
        i++;
        voice++;
        mask <<= 1;
    } while (i < 12);
}
