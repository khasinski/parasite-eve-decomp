void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *(unsigned char *)ptr;
    unsigned int value = ((right >> 2) << 15) | (left << 8);

    *ptr = current | value;
}
