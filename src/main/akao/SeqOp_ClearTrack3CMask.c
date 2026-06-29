
typedef unsigned int u32;

extern char *g_AkaoCurTrack;
extern u32 g_AkaoTrack3CMask;

void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_ClearTrack3CMask(void *ptr, u32 mask) {
    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        *(u32 *)(g_AkaoCurTrack + 0x3C) &= ~mask;
    } else {
        g_AkaoTrack3CMask &= ~mask;
    }

    Seq_MarkTrack3CMaskDirty();
    *(unsigned short *)((char *)ptr + 0xBC) = 0;
}
