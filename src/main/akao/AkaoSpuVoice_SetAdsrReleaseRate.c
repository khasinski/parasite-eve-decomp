typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetAdsrReleaseRate(u32 index, u32 left, u32 right) {
    register u16 *ptr asm("$4");
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C0A + (index * 0x10));
    asm("" : "=r"(ptr) : "0"(ptr));
    right >>= 2;
    right <<= 5;
    current = *ptr;
    value = right | left;
    current &= 0xFFC0;
    *ptr = current | value;
}
