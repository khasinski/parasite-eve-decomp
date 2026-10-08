#ifndef PE1_PSYQ_SPU_API_H
#define PE1_PSYQ_SPU_API_H

/* Psy-Q LIBSPU public stereo volume and external-input attributes. */
typedef struct SpuVolume {
    short left, right;
} SpuVolume;
typedef struct SpuExtAttr {
    SpuVolume volume;
    int reverb, mix;
} SpuExtAttr;
typedef struct SpuCommonAttr {
    unsigned int mask;
    /* 0x04 */ SpuVolume mvol;
    /* 0x08 */ SpuVolume mvolmode;
    /* 0x0C */ SpuVolume mvolx;
    /* 0x10 */ SpuExtAttr cd;
    /* 0x1C */ SpuExtAttr ext;
} SpuCommonAttr;
typedef char SpuVolumeSizeCheck[(sizeof(SpuVolume) == 4) ? 1 : -1];
typedef char SpuExtAttrSizeCheck[(sizeof(SpuExtAttr) == 12) ? 1 : -1];
typedef char SpuCommonAttrSizeCheck[(sizeof(SpuCommonAttr) == 40) ? 1 : -1];

void SpuSetCommonAttr(SpuCommonAttr *attr);
int SpuSetReverb(int on_off);
int SpuSetReverbModeType(unsigned int mode);
void SpuInit(void);
void SpuStart(void);
void SpuQuit(void);

#endif
