extern char *g_AkaoCurTrack;
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;

void AkaoSpuVoice_SetVolume(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetPitch(unsigned int index, unsigned int value);
void AkaoSpuVoice_SetAdsrAttack(unsigned int index, unsigned int left, unsigned int right);
void AkaoSpuVoice_SetAdsrSustainRate(unsigned int index, unsigned int left, unsigned int right);

void Seq_DeactivatePendingTracks(void) {
    unsigned int pending;
    unsigned int mask;
    unsigned int bit;
    unsigned int index;

    if (*(unsigned int *)(g_AkaoCurTrack + 4) != 0) {
        mask = ~g_SpuActiveVoiceMask & 0xFFFFFF;
        if (mask != 0) {
            bit = 1;
            index = 0;
            do {
                if (mask & bit) {
                    AkaoSpuVoice_SetVolume(index, 0, 0);
                    AkaoSpuVoice_SetPitch(index, 0);
                    AkaoSpuVoice_SetAdsrAttack(index, 0x7F, 1);
                    AkaoSpuVoice_SetAdsrSustainRate(index, 0x7F, 3);
                    mask &= ~bit;
                }
                bit <<= 1;
                index++;
            } while (mask != 0);
        }

        pending = *(unsigned int *)(g_AkaoCurTrack + 4);
        *(unsigned int *)(g_AkaoCurTrack + 4) = 0;
        *(unsigned int *)(g_AkaoCurTrack + 0x1C) = pending;
    }

    g_AkaoSeqPendingFlags |= 1;
}
