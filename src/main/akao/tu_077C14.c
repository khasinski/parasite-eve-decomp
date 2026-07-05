#include "include_asm.h"

extern int g_AkaoStreamUploadMode;

extern volatile unsigned short D_1F801D88;
extern volatile unsigned short D_1F801D8A;

extern volatile unsigned short D_1F801D8C;
extern volatile unsigned short D_1F801D8E;

extern volatile unsigned short D_1F801D98;
extern volatile unsigned short D_1F801D9A;

extern volatile unsigned short D_1F801D94;
extern volatile unsigned short D_1F801D96;

extern volatile unsigned short D_1F801D90;
extern volatile unsigned short D_1F801D92;

int Spu_SetStreamModeB(void) {
    g_AkaoStreamUploadMode = 2;
    return 0;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/tu_077C14", Spu_UploadStreamBlockA);

INCLUDE_ASM("asm/USA/main/nonmatchings/akao/tu_077C14", Spu_UploadStreamBlockB);

void Spu_WriteKeyOn(unsigned int value) {
    D_1F801D88 = value;
    value >>= 16;
    D_1F801D8A = value;
}

void Spu_WriteKeyOff(unsigned int value) {
    D_1F801D8C = value;
    value >>= 16;
    D_1F801D8E = value;
}

void Spu_WriteReverbEnable(unsigned int value) {
    D_1F801D98 = value;
    value >>= 16;
    D_1F801D9A = value;
}

void Spu_WriteNoiseEnable(unsigned int value) {
    D_1F801D94 = value;
    value >>= 16;
    D_1F801D96 = value;
}

void Spu_WriteFmEnable(unsigned int value) {
    D_1F801D90 = value;
    value >>= 16;
    D_1F801D92 = value;
}

void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right)
{
  unsigned int new_var;
  unsigned short *ptr = (unsigned short *) (0x1F801C00 + (((unsigned long long) index) * 0x10));
  new_var = 0x7FFF;
  ptr[0] = left & new_var;
  ptr[1] = right & 0x7FFF;
}

void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C04 / 2] = value;
}

void AkaoSpuVoice_SetStartAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C06 / 2] = value >> 3;
}

void AkaoSpuVoice_SetRepeatAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C0E / 2] = value >> 3;
}

void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *(unsigned char *)ptr;
    unsigned int value = ((right >> 2) << 15) | (left << 8);

    *ptr = current | value;
}

void AkaoSpuVoice_SetAdsrDecayRate(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFF0F) | (value << 4);
}

void AkaoSpuVoice_SetAdsrSustainLevel(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFFF0) | value;
}

void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 1) << 14) | (left << 6);
    unsigned int current = *ptr;

    *ptr = (current & 0x3F) | value;
}

void AkaoSpuVoice_SetAdsrReleaseRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 2) << 5) | left;
    unsigned int current = *ptr;

    *ptr = (current & 0xFFC0) | value;
}
