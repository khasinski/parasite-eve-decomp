#ifndef PE1_AKAO_SPU_COMMON_H
#define PE1_AKAO_SPU_COMMON_H

/* Common SPU settings; volume words also encode signed sweep rates. */
typedef struct SpuCommonSettings {
    /* 0x00 */ unsigned int mask;
    /* 0x04 */ unsigned short left, right;
    /* 0x08 */ short leftMode, rightMode;
    /* 0x0C */ short currentLeft, currentRight;
    /* 0x10 */ short cdLeft, cdRight;
    /* 0x14 */ int cdReverb, cdMix;
    /* 0x1C */ short externalLeft, externalRight;
    /* 0x20 */ int externalReverb, externalMix;
} SpuCommonSettings;

/* Historical symbol name: this writes common, not per-voice, registers. */
void SPU_WriteVoiceRegs(SpuCommonSettings *attr);

#endif
