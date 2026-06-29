/* CC1_PROFILE: build0001 */

extern char *g_AkaoCurTrack;
extern unsigned int g_AkaoSeqPendingFlags;
extern int g_AkaoTrackStateArray[];

void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void Seq_RestorePendingTracks(void) {
    unsigned int pending = *(unsigned int *)(g_AkaoCurTrack + 0x1C);
    unsigned int saved;
    unsigned int mask;
    int *slot;

    if (pending != 0) {
        mask = 1;
        slot = g_AkaoTrackStateArray;
        do {
            if (pending & mask) {
                pending &= ~mask;
                *slot |= 0x2B13;
            }
            mask <<= 1;
            slot = (int *)((char *)slot + 0x11C);
        } while (pending != 0);

        saved = *(unsigned int *)(g_AkaoCurTrack + 0x1C);
        *(unsigned int *)(g_AkaoCurTrack + 0x1C) = 0;
        *(unsigned int *)(g_AkaoCurTrack + 4) = saved;
        Seq_MarkTrack34MaskDirty();
        Seq_MarkTrack38MaskDirty();
        Seq_MarkTrack3CMaskDirty();
    }

    g_AkaoSeqPendingFlags &= ~1;
}
