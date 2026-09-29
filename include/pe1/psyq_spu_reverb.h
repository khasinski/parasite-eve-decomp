#ifndef PE1_PSYQ_SPU_REVERB_H
#define PE1_PSYQ_SPU_REVERB_H

/* Reverb preset packet passed from LIBSPU's preset selector to the register writer. */
typedef struct SpuReverbRegisterAttrs {
    unsigned int mask;
    unsigned short regs[32];
} SpuReverbRegisterAttrs;

extern unsigned int g_SpuReverbWorkAreaTable[];
extern SpuReverbRegisterAttrs g_SpuReverbPresetRegisters[];

int _SpuIsInAllocateArea_(unsigned int address);
void _spu_setReverbAttr(SpuReverbRegisterAttrs *attr);
int SpuClearReverbWorkArea(int mode);

#endif
