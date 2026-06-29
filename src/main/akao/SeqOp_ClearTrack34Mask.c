
typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern u32 g_AkaoTrack34Mask;

void Seq_MarkTrack34MaskDirty(void);

void SeqOp_ClearTrack34Mask(void *ptr, u32 mask) {
    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        *(u32 *)(g_AkaoCurTrack + 0x34) &= ~mask;
    } else {
        g_AkaoTrack34Mask &= ~mask;
    }

    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    *(unsigned short *)((char *)ptr + 0xBA) = 0;
}
