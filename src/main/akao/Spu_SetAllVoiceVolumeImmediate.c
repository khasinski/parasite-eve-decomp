typedef unsigned int u32;
typedef unsigned short u16;

extern char g_SpuVoiceControlTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetAllVoiceVolumeImmediate(int *arg0) {
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
                value = arg0[1];
                dirty = *(volatile u32 *)voice;
                *(u16 *)(voice - 0x80) = 0;
                value &= 0x7F;
                value <<= 8;
                dirty |= 3;
                *(u16 *)(voice - 0x1C) = value;
                *(u32 *)voice = dirty;
            }
        }
        i++;
        voice += 0x11C;
        mask <<= 1;
    } while (i < 12);
}
