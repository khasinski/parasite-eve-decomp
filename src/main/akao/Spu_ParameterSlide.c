/* MASPSX_FLAGS: --expand-div */
typedef unsigned int u32;
typedef unsigned short u16;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;

void Spu_ParameterSlide(int *arg0) {
    register char *base asm("$3");
    u32 active;
    u32 mask;
    u32 i;
    register char *voice asm("$6");
    int step;
    register int delta asm("$3");
    int denom;

    base = g_AkaoVoiceChannelTable;
    active = g_SpuActiveVoiceMask;
    mask = 0x1000;

    if (arg0[2] != 0) {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if ((*(u32 *)(voice - 0x48) & arg0[2]) != 0) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = arg0[4];
                    denom = *(u16 *)(voice + 0x64);
                    delta &= 0x7F;
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(u16 *)voice = step;
                    *(u16 *)(voice + 0x66) = delta;
                }
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
    } else {
        i = 0;
        voice = base + 0x74;
        do {
            if ((active & mask) != 0) {
                if (*(int *)(voice - 0x4C) == arg0[1]) {
                    step = 1;
                    if (arg0[3] != 0) {
                        step = arg0[3];
                    }
                    delta = arg0[4];
                    denom = *(u16 *)(voice + 0x64);
                    delta &= 0x7F;
                    delta <<= 8;
                    delta -= denom;
                    delta = (short)delta;
                    denom = (short)step;
                    delta = delta / denom;
                    *(u16 *)voice = step;
                    *(u16 *)(voice + 0x66) = delta;
                }
            }
            i++;
            voice += 0x11C;
            mask <<= 1;
        } while (i < 12);
    }
}
