typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetAdsrSustainLevel(u32 index, u32 value) {
    u16 *ptr;
    u32 current;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    current = *ptr;
    current &= 0xFFF0;
    *ptr = current | value;
}
