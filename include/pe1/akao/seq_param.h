#ifndef PE1_AKAO_SEQ_PARAM_H
#define PE1_AKAO_SEQ_PARAM_H

/* Re-applies SPU reverb (off, DMA step with param | 0x100, on) when param
 * differs from the current global pitch value. */
void Seq_SetParamWithReset(unsigned int param);

#endif
