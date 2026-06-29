void AkaoSpuVoice_SetStartAddress(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C06 / 2] = value >> 3;
}
