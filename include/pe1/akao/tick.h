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
        /* 0x48 */ unsigned char pad_48[0x0A];
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

typedef union AkaoTickTrack {
    AkaoTrack track;
    AkaoU32 words[sizeof(AkaoTrack) / 4];
    struct {
        /* 0x000 */ unsigned char pad_000[0x2C];
        /* 0x02C */ AkaoU32 key_on_mask;
        /* 0x030 */ unsigned char pad_030[0x20];
        /* 0x050 */ AkaoU32 tick_count;
        /* 0x054 */ AkaoU16 parent_track_id;
        /* 0x056 */ AkaoU16 note_length;
        /* 0x058 */ AkaoU16 gate_length;
    } timing;
} AkaoTickTrack;

PE1_STATIC_ASSERT(sizeof(AkaoTickBank) == 0x68, akao_tick_bank_size);
PE1_STATIC_ASSERT(sizeof(AkaoTickTrack) == 0x11C, akao_tick_track_size);

extern AkaoTickBank *D_8009D2C8;
extern AkaoTickTrack D_800B8AC0[48];
extern AkaoTickTrack D_800BC000[];
extern u8 D_8009D2D2;
extern u32 D_8009D2DC;
extern u32 D_8009D22C;
extern u32 D_8009D268;
extern u32 D_8009D2C4;
extern u32 D_800BCD50;
extern u32 D_800BCD58;
extern u32 D_800BCD5C;
extern u16 D_800BCD66;
extern u32 D_800BCD68;

void Akao_ProcessVoiceQueue(void);
void Akao_StepSampleLoader(AkaoTrack *track, u32 voice_mask);
void Spu_UpdateVoiceRegisters(AkaoTrack *track, u32 voice_mask);
void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned int voice_mask);
void Akao_ProcessMessageQueue(void);
void SPU_StepReverbLoad(void);
void Akao_SetVoicePitch(void);
void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
void Akao_Tick(void);

#endif
