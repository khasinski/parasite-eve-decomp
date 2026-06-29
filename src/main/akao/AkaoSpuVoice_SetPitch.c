void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F800000 + index * 0x10);

    ptr[0x1C04 / 2] = value;
}
