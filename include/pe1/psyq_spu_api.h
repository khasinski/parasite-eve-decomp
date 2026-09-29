#ifndef PE1_PSYQ_SPU_API_H
#define PE1_PSYQ_SPU_API_H

/* Psy-Q LIBSPU common volume and mix attributes. */
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
int SpuSetReverb(int on_off);
int SPU_StepDmaRead(unsigned int mode);
void SpuInit(void);
void SpuStart(void);
void SpuQuit(void);

#endif
