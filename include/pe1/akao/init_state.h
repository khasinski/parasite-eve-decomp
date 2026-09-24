#ifndef PE1_AKAO_INIT_STATE_H
#define PE1_AKAO_INIT_STATE_H

#include "common.h"
#include "pe1/akao/queue.h"
#include "pe1/akao/spu_common.h"

extern u32 D_8009D200;
extern u32 D_800BCD50;
extern u8 D_800B6980[];
extern u8 D_800B8AC0[];
extern u8 D_800BC03C[];
extern u8 D_800C0D90[];

void Spu_InitVoiceState(void);
void Spu_SetVoiceAttr(int voice, u16 left, u16 right, s16 left_mode, u16 right_mode);
void Seq_SetParamWithReset(unsigned int param);

#endif
