/* CC1_PROFILE: build0001 */

extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuPendingKeyOffMask;
extern unsigned int g_AkaoVoiceUpdateFlags;
extern char g_AkaoVoiceChannelTable[];

void SeqOp_DeactivateVoice(void *, unsigned int);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Spu_StopActiveVoices(void) {
    char *voice = g_AkaoVoiceChannelTable;
    unsigned int mask = 0x1000;
    unsigned int i = 0;
    unsigned int skip_flag = 0x02000000;
    char *field_38 = voice + 0x38;

    do {
        if (g_SpuActiveVoiceMask & mask) {
            if ((*(int *)(field_38 - 0xC) & skip_flag) == 0) {
                g_SpuPendingKeyOffMask |= mask;
                SeqOp_DeactivateVoice(voice, mask);
                *(int *)field_38 = 0;
            }
        }
        i++;
        field_38 += 0x11C;
        voice += 0x11C;
        mask <<= 1;
    } while (i < 0xC);

    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
