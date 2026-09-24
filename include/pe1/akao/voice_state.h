#ifndef PE1_AKAO_VOICE_STATE_H
#define PE1_AKAO_VOICE_STATE_H

#include "pe1/akao/track.h"

extern AkaoSequencerBank g_AkaoTrackStateBackup;
extern AkaoVoiceBank g_AkaoVoiceStateBackup;
extern AkaoVoiceBank g_AkaoVoiceStateTable;
extern AkaoTrack g_AkaoVoiceStateTable2[];
extern AkaoSequencerBank *g_AkaoCurTrack;
extern unsigned int g_AkaoVoiceKeyOnState;
extern unsigned int g_AkaoVoiceUpdateFlags;
extern char g_AkaoDefaultVoiceProgram[];
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuPendingKeyOffMask;
extern unsigned int D_800BCD6C, D_800BCD74;
extern unsigned int g_AkaoSeqPendingFlags;
extern unsigned short g_AkaoSelectedBankId;

void Util_CopyWords(unsigned int *src, unsigned int *dst, unsigned int size);
unsigned int Akao_ForEachVoiceMasked(AkaoTrack *track, unsigned int mask);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);
void Spu_TickVoiceEnvelopes(AkaoTrack *track, unsigned int voice_mask);

#endif
