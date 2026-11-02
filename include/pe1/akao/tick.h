#ifndef PE1_AKAO_TICK_H
#define PE1_AKAO_TICK_H

#include "common.h"
#include "pe1/akao/track.h"

/* Views used by the AKAO timer tick (Akao_Tick). They overlay the shared
 * sequencer bank and track records and name the timing fields that the
 * shared AkaoSequencerBank/AkaoTrack layouts leave anonymous. */

/* 16.16 tempo: the tick adds the integer half to the beat accumulator. */
typedef union AkaoTickTempo {
    AkaoU32 value;
    struct {
        AkaoU16 fraction;
        AkaoU16 integer;
    } half;
} AkaoTickTempo;

typedef union AkaoTickBank {
    AkaoSequencerBank bank;
    AkaoU32 words[sizeof(AkaoSequencerBank) / 4];
    struct {
        /* 0x00 */ AkaoU32 status_flags;
        /* 0x04 */ AkaoU32 active_voice_mask;
        /* 0x08 */ unsigned char pad_08[0x0C];
        /* 0x14 */ AkaoU32 allocated_voice_mask;
        /* 0x18 */ AkaoU32 key_off_request_mask;
        /* 0x1C */ AkaoU32 pending_restore_mask;
        /* 0x20 */ AkaoTickTempo tempo;
        /* 0x24 */ AkaoU32 tempo_delta;
        /* 0x28 */ AkaoU32 tick_accumulator;
        /* 0x2C */ unsigned char pad_2C[0x14];
        /* 0x40 */ AkaoU32 volume;
        /* 0x44 */ AkaoU32 volume_delta;
        /* 0x48 */ AkaoU16 pitch_fraction; /* halves of the 16.16 bank pitch */
        /* 0x4A */ AkaoU16 pitch_integer;
        /* 0x4C */ unsigned char pad_4C[0x06];
        /* 0x52 */ AkaoU16 tempo_slide_duration;
        /* 0x54 */ AkaoU16 bank_id;
        /* 0x56 */ AkaoU16 pad_56;
        /* 0x58 */ AkaoU16 volume_slide_duration;
        /* 0x5A */ AkaoU16 pad_5A;
        /* 0x5C */ AkaoU16 beats_per_measure;
        /* 0x5E */ AkaoU16 beat;
        /* 0x60 */ AkaoU16 ticks_per_beat;
        /* 0x62 */ AkaoU16 tick;
        /* 0x64 */ AkaoU16 measure;
        /* 0x66 */ AkaoU16 pad_66;
    } timing;
} AkaoTickBank;

PE1_STATIC_ASSERT(sizeof(AkaoTickBank) == 0x68, akao_tick_bank_size);

extern AkaoTickBank *D_8009D2C8;
extern AkaoTickBank *D_8009CD88;
extern u8 g_AkaoTempoScale;
extern u32 g_AkaoSeqPendingFlags;
/* Message-queue lock, also read by the timer interrupt. */
extern volatile u32 g_AkaoMessageQueueLock;
extern u32 g_AkaoVoiceUpdateFlags;
extern u32 g_SpuActiveVoiceMask;
extern u32 g_AkaoSfxAllocatedVoiceMask;
extern u32 g_SpuPendingKeyOffMask;
/* Sound-effect tempo, 16.16; the tick adds its integer half. */
extern AkaoTickTempo g_AkaoSfxTempo;
extern u32 g_AkaoSfxTickAccumulator;

void Akao_ProcessVoiceQueue(void);
void Akao_StepSampleLoader(AkaoTrack *track, u32 voice_mask);
void Spu_UpdateVoiceRegisters(AkaoTrack *track, u32 voice_mask);
void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned int voice_mask);
void Akao_ProcessMessageQueue(void);
void Spu_StepReverbLoad(void);
void Akao_SetVoicePitch(void);
void Akao_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
void Akao_Tick(void);
int Akao_TimerCallback(void);

#endif
