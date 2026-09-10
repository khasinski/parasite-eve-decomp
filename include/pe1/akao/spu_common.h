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

void SpuSetCommonAttr(SpuCommonSettings *attr);

/* Historical symbol name: set reverb enable and return its resulting state. */
int SpuSetReverb(int on_off);

typedef struct SpuReverbRegisterAttrs {
    unsigned int mask;
    unsigned short regs[32];
} SpuReverbRegisterAttrs;

void _spu_setReverbAttr(SpuReverbRegisterAttrs *attr);
/* Historical symbol name: select a reverb preset, optionally clearing its RAM. */
int SPU_StepDmaRead(unsigned int mode);
int SpuClearReverbWorkArea(int mode);

#endif
