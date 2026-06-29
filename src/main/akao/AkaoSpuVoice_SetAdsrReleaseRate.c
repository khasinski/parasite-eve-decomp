void AkaoSpuVoice_SetAdsrReleaseRate(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C0A + index * 0x10);
    unsigned int value = ((right >> 2) << 5) | left;
    unsigned int current = *ptr;

    *ptr = (current & 0xFFC0) | value;
}
