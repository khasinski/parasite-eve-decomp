typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoicePitchImmediate(int *arg0) {
    u32 mask;
    u32 active;
    u32 i;
    u32 block_flag;
    register char *voice asm("$5");
    int value;
    int dirty;

    mask = 0x1000;
    active = g_SpuActiveVoiceMask;
    i = 0;
    block_flag = 0x02000000;
    voice = g_SpuVoiceControlTable;

    do {
        if ((active & mask) != 0) {
            if ((*(u32 *)(voice - 0xC8) & block_flag) == 0) {
                value = ((u8 *)arg0)[4];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x84) = 0;
                value <<= 8;
                dirty |= 0x10;
                *(u32 *)(voice - 0xB8) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += 0x11C;
        mask <<= 1;
    } while (i < 12);
}
