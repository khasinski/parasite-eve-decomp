void AkaoSpuVoice_SetRepeatAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C0E / 2] = value >> 3;
}
