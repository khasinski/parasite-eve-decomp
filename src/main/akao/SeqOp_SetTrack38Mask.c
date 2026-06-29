
typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern u32 g_AkaoTrack38Mask;

void Seq_MarkTrack38MaskDirty(void);

void SeqOp_SetTrack38Mask(void *ptr, u32 mask) {
    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        *(u32 *)(g_AkaoCurTrack + 0x38) |= mask;
    } else {
        g_AkaoTrack38Mask |= mask;
    }

    Seq_MarkTrack38MaskDirty();
}
