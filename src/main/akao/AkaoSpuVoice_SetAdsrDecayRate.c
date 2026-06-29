void AkaoSpuVoice_SetAdsrDecayRate(unsigned int index, unsigned int value) {
    unsigned short *ptr = (unsigned short *)(0x1F801C08 + index * 0x10);
    unsigned int current = *ptr;

    *ptr = (current & 0xFF0F) | (value << 4);
}
