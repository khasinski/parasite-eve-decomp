#ifndef PE1_AKAO_NOTE_STEP_H
#define PE1_AKAO_NOTE_STEP_H

/* Declarations used by the sequencer note step Akao_StepSampleLoader. */

#include "pe1/akao/track.h"
#include "pe1/akao/tick.h"

typedef void (*AkaoCommandHandler)(AkaoTrack *track, u32 voice_mask);

/* Opcode handlers: 0xA0..0xFF, and the 0xFC extended page. */
extern AkaoCommandHandler D_8009C8F0[];
extern AkaoCommandHandler D_8009CCF0[];
/* Note lengths in ticks, indexed by opcode % 11. */
extern u16 D_8009B8DC[];
/* LFO waveform tables. */
extern void *D_8009C080[];
/* Key-on mask of the child (sound effect) tracks. */
extern u32 D_800BCD54;
/* Instrument table, 0x40-byte records (also viewed as words by the
 * stream relocation code in akao.h). */
extern AkaoInstrument D_800B2900[];

int Akao_LookupSampleBankByte(AkaoTrack *track);
int Akao_LookupPitchPeriod(int instrument, int note, int detune);
void Akao_SetNotePitchBounded(AkaoTrack *track, int note);
void Seq_MarkTrack38MaskDirty(void);
void SeqOp_SetVoiceInstrument(AkaoTrack *track, AkaoInstrument *instrument,
                              int sample_header);

#endif /* PE1_AKAO_NOTE_STEP_H */
