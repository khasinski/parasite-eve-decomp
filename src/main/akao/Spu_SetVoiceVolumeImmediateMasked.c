typedef unsigned int u32;
typedef unsigned short u16;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_SetVoiceVolumeImmediateMasked(int *arg0) {
    u32 i;
    u32 mask;
    u32 active;
    register char *voice asm("$5");
    register char *base asm("$3");
    int value;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = 0x1000;
    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0xC8) & arg0[2]) != 0) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16 *)(voice - 0x80) = 0;
                    *(u16 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= 3;
                }
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0xF4;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0xCC) == arg0[1]) {
                    value = (arg0[3] & 0x7F) << 8;
                    *(u16 *)(voice - 0x80) = 0;
                    *(u16 *)(voice - 0x1C) = value;
                    *(u32 *)voice |= 3;
                }
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
    }
}
