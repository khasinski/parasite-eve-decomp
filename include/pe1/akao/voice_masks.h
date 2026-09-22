#ifndef PE1_AKAO_VOICE_MASKS_H
#define PE1_AKAO_VOICE_MASKS_H

#include "pe1/akao/track.h"

extern AkaoSequencerBank *g_AkaoCurTrack;
extern AkaoTrack g_AkaoVoiceStateTable[], g_AkaoVoiceStateTable2[];
extern unsigned g_SpuActiveVoiceMask, g_SpuStoppedVoiceMask;
extern unsigned g_SpuPendingKeyOffMask;
extern unsigned g_AkaoTrack34Mask, g_AkaoTrack38Mask, g_AkaoTrack3CMask;
extern unsigned g_AkaoVoiceUpdateFlags;
extern unsigned D_800C0DD0, D_800C0DD4, D_800C0DD8;

void Spu_VoiceMaskCompose(AkaoTrack *track, int *mask_out, int mask, int mask_keep);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);
void Spu_WriteKeyOff(unsigned value);
void Akao_SetVoicePitch(void);
void Akao_SetVoiceVolume(void);
void Akao_SetVoiceAdsr(void);
void Akao_SetVoiceStartAddr(void);

#endif
