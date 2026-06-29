void AkaoSpuVoice_SetAdsrSustainLevel(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFFF0) | value;
}
