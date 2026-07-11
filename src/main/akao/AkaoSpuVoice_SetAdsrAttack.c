typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetAdsrAttack(u32 index, u32 left, u32 right) {
    register u16 *ptr asm("$4");
    u32 current;
    u32 value;

    ptr = (u16 *)(0x1F801C08 + (index * 0x10));
    asm("" : "=r"(ptr) : "0"(ptr));
    right >>= 2;
    right <<= 15;
    left <<= 8;
    current = *(u8 *)ptr;
    value = right | left;
    *ptr = current | value;
}
