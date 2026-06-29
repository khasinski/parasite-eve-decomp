/* CC1_PROFILE: build0001 */
typedef unsigned int u32;

extern char g_AkaoVoiceChannelTable[];
extern u32 g_SpuActiveVoiceMask;
extern u32 g_SpuStoppedVoiceMask;
extern u32 g_AkaoSeqPendingFlags;

void AkaoSpuVoice_SetVolume(u32 index, u32 left, u32 right);
void AkaoSpuVoice_SetPitch(u32 index, u32 value);
void AkaoSpuVoice_SetAdsrAttack(u32 index, u32 left, u32 right);
void AkaoSpuVoice_SetAdsrSustainRate(u32 index, u32 left, u32 right);

void Spu_VoiceStopAll(void) {
    u32 pending;
    u32 bit;
    u32 voice_index;
    register u32 *active_ptr asm("$2");
    u32 active;
    u32 inverse;
    char *voice;
    u32 i;

    pending = g_SpuActiveVoiceMask;
    if (pending != 0) {
        voice = g_AkaoVoiceChannelTable;
        bit = 0x1000;
        i = 0;
        do {
            if ((pending & bit) != 0) {
                if ((*(u32 *)(voice + 0x2C) & 0x02000000) != 0) {
                    pending &= ~bit;
                }
            }
            i++;
            voice += 0x11C;
            bit <<= 1;
        } while (i < 12);

        bit = 0x1000;
        voice_index = 12;
        active_ptr = &g_SpuActiveVoiceMask;
        g_SpuStoppedVoiceMask = pending;
        active = *active_ptr;
        inverse = ~pending;
        active &= inverse;
        *active_ptr = active;

        if (pending != 0) {
            do {
                if ((pending & bit) != 0) {
                    AkaoSpuVoice_SetVolume(voice_index, 0, 0);
                    AkaoSpuVoice_SetPitch(voice_index, 0);
                    AkaoSpuVoice_SetAdsrAttack(voice_index, 0x7F, 1);
                    AkaoSpuVoice_SetAdsrSustainRate(voice_index, 0x7F, 3);
                    pending &= ~bit;
                }
                bit <<= 1;
                voice_index++;
            } while (pending != 0);
        }
    }

    g_AkaoSeqPendingFlags |= 2;
}
