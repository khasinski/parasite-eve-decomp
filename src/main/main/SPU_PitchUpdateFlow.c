#include "common.h"
#include "pe1/akao/spu_common.h"
#include "pe1/psyq_spu_internal.h"
#include "pe1/akao/track.h"


extern unsigned short D_8009D2B6;
extern SpuCommonSettings D_800C0D90;
/* These field aliases keep the retail absolute stores byte-identical. */
extern unsigned short D_800C0DA0;
extern unsigned short D_800C0DA2;
extern int D_800C0DA4;

void Seq_GetGlobalPitch(unsigned int *out)
{
    *out = D_8009B3A0.mode;
}

void Seq_ApplyGlobalPitch(void) {
    unsigned short value = D_8009D2B6;
    char *regs;

    __asm__("" : "=r"(regs) : "0"(&D_800C0D90));
    *(int *)regs = 0x1C0;
    D_800C0DA4 = 0;
    D_800C0DA2 = value;
    D_800C0DA0 = value;
    SpuSetCommonAttr((SpuCommonSettings *)regs);
}

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size)
{
    size >>= 2;
    do {
        *dst = *src;
        src++;
        size--;
        dst++;
    } while (size != 0);
}
extern u16 D_8009CDEC;
extern s16 D_8009D2A2, D_8009D220, D_8009D21E;
extern u32 D_8009D2B4, D_8009D284, D_8009D2D0, D_8009D214;
extern u32 D_8009D2CC, D_8009D210, D_800BCD50;
extern AkaoSequencerBank *volatile D_8009D2C8;
extern AkaoTrack D_800B8AC0[], D_800BA560[], D_800BC000[];
void Seq_ApplyGlobalPitch(void);
void func_8008AB9C(AkaoTrack *);

/* Advance global, bank and voice slides every fourth tick.
 * Matching debt: old is pinned to v1; the empty memory barrier preserves
 * the cursor reload after the callback-dependent bank store. */
void SPU_StepReverbLoad(void)
{
    u32 value;
    register u32 old asm("$3");
    u32 pending;
    int count;
    AkaoTrack *track;
    AkaoSequencerBank *bank;
    if ((++D_8009CDEC & 3) != 0)
        return;
    if (D_8009D2A2 != 0) {
        D_8009D2A2--;
        D_8009D2B4 += D_8009D284;
        Seq_ApplyGlobalPitch();
    }
    if (D_8009D220 != 0) {
        D_8009D220--;
        D_8009D2D0 += D_8009D214;
    }
    if (D_8009D21E != 0) {
        old = D_8009D2CC;
        D_8009D21E--;
        value = old + D_8009D210;
        if ((value & 0xFF0000) != (old & 0xFF0000)) {
            track = D_800B8AC0;
            for (count = 24; count != 0; --count, ++track)
                track->update_flags |= 0x10;
        }
        D_8009D2CC = value;
    }
    bank = D_8009D2C8;
    if (bank->active_voice_mask && bank->pitch_slide_duration) {
        old = bank->pitch_current;
        bank->pitch_slide_duration--;
        value = old + bank->pitch_delta;
        if ((value & 0x7F0000) != (old & 0x7F0000))
            func_8008AB9C(D_800B8AC0);
        D_8009D2C8->pitch_current = value;
        asm volatile("" ::: "memory");
        bank = D_8009D2C8;
    }
    D_8009D2C8 = bank + 1;
    if (bank[1].active_voice_mask && bank[1].pitch_slide_duration) {
        old = bank[1].pitch_current;
        bank[1].pitch_slide_duration--;
        value = old + bank[1].pitch_delta;
        if ((value & 0x7F0000) != (old & 0x7F0000))
            func_8008AB9C(D_800BA560);
        D_8009D2C8->pitch_current = value;
    }
    D_8009D2C8 = D_8009D2C8 - 1;
    pending = D_800BCD50;
    if (pending) {
        count = 0x1000;
        track = D_800BC000;
        do {
            if (pending & count) {
                if (track->panpot_duration) {
                    old = (s16)track->pan_target;
                    track->panpot_duration--;
                    value = old + (s16)track->pan_delta;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 3;
                    track->pan_target = value;
                }
                if (track->panpot_slide_duration) {
                    old = track->panpot;
                    track->panpot_slide_duration--;
                    value = old + (s16)track->panpot_delta;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 3;
                    track->panpot = value;
                }
                if (track->field_70) {
                    old = track->voice_mask_b;
                    track->field_70--;
                    value = old + track->pan_base;
                    if ((value & 0xFF00) != (old & 0xFF00))
                        track->update_flags |= 0x10;
                    track->voice_mask_b = value;
                }
                pending ^= count;
            }
            count = count << 1;
            ++track;
        } while (pending);
    }
}
