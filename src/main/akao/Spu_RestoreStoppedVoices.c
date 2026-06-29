/* CC1_PROFILE: build0001 */

extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_SpuStoppedVoiceMask;
extern unsigned int g_AkaoSeqPendingFlags;
extern int g_SpuVoiceControlTable[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Spu_RestoreStoppedVoices(void) {
    unsigned int pending = g_SpuStoppedVoiceMask;
    unsigned int saved;
    unsigned int mask;
    int *slot;

    if (pending != 0) {
        mask = 0x1000;
        slot = g_SpuVoiceControlTable;
        do {
            if (pending & mask) {
                pending &= ~mask;
                *slot |= 0x2B13;
            }
            mask <<= 1;
            slot = (int *)((char *)slot + 0x11C);
        } while (pending != 0);

        saved = g_SpuStoppedVoiceMask;
        g_SpuStoppedVoiceMask = 0;
        g_SpuActiveVoiceMask = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~2;
}
