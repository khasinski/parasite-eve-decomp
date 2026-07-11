typedef unsigned short u16;
typedef unsigned int u32;

void AkaoSpuVoice_SetStartAddress(u32 index, u32 value) {
    u16 *ptr;

    ptr = (u16 *)(0x1F800000 + (index * 0x10));
    ptr[0x1C06 / 2] = value >> 3;
}
