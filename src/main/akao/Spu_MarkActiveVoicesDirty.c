/* CC1_PROFILE: build0001 */
typedef unsigned int u32;

extern u32 g_SpuActiveVoiceMask;
extern char g_AkaoVoiceChannelTable[];

void Spu_MarkActiveVoicesDirty(void) {
    u32 mask;
    u32 bit;
    char *base;
    char *ptr;

    mask = g_SpuActiveVoiceMask;
    base = g_AkaoVoiceChannelTable;
    if (mask != 0) {
        bit = 0x1000;
        ptr = base + 0xF4;
        do {
            if (mask & bit) {
                mask ^= bit;
                *(u32 *)ptr |= 3;
            }
            ptr += 0x11C;
            bit <<= 1;
        } while (mask != 0);
    }
}
