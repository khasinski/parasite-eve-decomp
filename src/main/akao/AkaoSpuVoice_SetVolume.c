typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetVolume(u32 index, u32 left, u32 right) {
    register u16 *ptr asm("$4");
    u32 mask;

    ptr = (u16 *)(0x1F801C00 + (index * 0x10));
    asm("" : "=r"(ptr) : "0"(ptr));
    mask = 0x7FFF;
    ptr[0] = left & mask;
    ptr[1] = right & 0x7FFF;
}
