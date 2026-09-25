#ifndef PE1_AKAO_VOICE_MASKS_H
#define PE1_AKAO_VOICE_MASKS_H

#include "pe1/akao/track.h"

extern AkaoSequencerBank *g_AkaoCurTrack;
extern AkaoTrack g_AkaoVoiceStateTable[], g_AkaoVoiceStateTable2[];
extern AkaoTrack g_AkaoVoiceChannelTable[];
extern unsigned g_SpuActiveVoiceMask, g_SpuStoppedVoiceMask;
extern unsigned g_SpuPendingKeyOffMask;
extern AkaoVoiceEnvelopeSlot g_AkaoVoiceEnvelopeTable[];
extern unsigned g_AkaoVoicePortamentoResetMask;
extern unsigned g_AkaoTrack34Mask, g_AkaoTrack38Mask, g_AkaoTrack3CMask;
extern unsigned g_AkaoVoiceUpdateFlags;
extern unsigned D_800C0DD0, D_800C0DD4, D_800C0DD8;
extern int D_8009D2C0;
extern short D_8009B8F8[], D_8009B9F8[];

void Akao_SetVoiceKeyOn(AkaoTrack *voice, unsigned int voice_mask);

void Spu_ManageVoices(int id, int control);
void Seq_StartNestedTrack(AkaoTrack *track, AkaoNestedSource *source, unsigned mask, void *script);
void Seq_ClearTrackVoiceId(AkaoTrack *track, unsigned voice_id);
void Seq_StartNestedStreams(AkaoNestedSource *source, void *first, void *second);

void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);
void Akao_SetVoiceKeyOff(AkaoTrack *track, unsigned mask, int index);
void Akao_StepVoiceNote(AkaoTrack *track, unsigned mask, unsigned direct, unsigned *output);
/* Retail callers supply differing numbers of trailing, unused arguments. */
void Akao_WriteVoiceParam();
void Spu_WriteKeyOff(unsigned value);
void Akao_SetVoicePitch(void);
void Akao_SetVoiceVolume(void);
void Akao_SetVoiceAdsr(void);
void Akao_SetVoiceStartAddr(void);

#endif
