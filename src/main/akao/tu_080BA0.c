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

void SeqOp_ClearMask(void *ptr, u32 mask) {
    char *track;

    if (*(unsigned short *)((char *)ptr + 0x54) == 0) {
        track = g_AkaoCurTrack;
        *(u32 *)(track + 0x38) &= ~mask;
        if ((*(u32 *)((char *)ptr + 0x38) & 0x800) != 0) {
            *(u32 *)(track + 0x30) &= ~(1 << *(unsigned short *)((char *)ptr + 0x5C));
        }
    } else {
        g_AkaoTrack38Mask &= ~mask;
    }

    Seq_MarkTrack38MaskDirty();
}

void SeqOp_EnableField84(void *arg0) {
    *(short *)((char *)arg0 + 0x84) = 1;
}

void SeqOp_Noop_904AC(void) {
}

void SeqOp_Noop_904B4(void) {
}

void SeqOp_Noop_904BC(void) {
}
