void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 1) << 14) | (left << 6);
    unsigned int current = *ptr;

    *ptr = (current & 0x3F) | value;
}
