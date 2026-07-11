typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetAdsrSustainRate(u32 index, u32 left, u32 right) {
    register u16 *ptr asm("$4");
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    asm("" : "=r"(ptr) : "0"(ptr));
    right >>= 1;
    right <<= 14;
    left <<= 6;
    current = *ptr;
    value = right | left;
    current &= 0x3F;
    *ptr = current | value;
}
