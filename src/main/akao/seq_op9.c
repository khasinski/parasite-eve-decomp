typedef unsigned int u32;
typedef unsigned short u16;

extern char *g_AkaoCurTrack;
extern u32 g_AkaoVoiceUpdateFlags;
extern int g_AkaoSeqLoopCounter;

void SeqOp_DeactivateVoice(void *ptr);
void Seq_MarkTrack34MaskDirty(void);
void Seq_MarkTrack38MaskDirty(void);
void Seq_MarkTrack3CMaskDirty(void);

void SeqOp_SetSustainFlag(void *ptr) {
    *(int *)((char *)ptr + 0x38) |= 0x100000;
}

void SeqOp_StopVoice(void *ptr, u32 arg1) {
    char *track;
    u32 mask;

    if (*(u16 *)((char *)ptr + 0x54) == 0) {
        track = g_AkaoCurTrack;
        mask = arg1 ^ ~0xFF000000u;

        *(u32 *)(track + 4) &= mask;
        if (*(u32 *)(track + 4) == 0) {
            g_AkaoSeqLoopCounter = 0;
            *(u16 *)(track + 0x54) = 0;
        }

        track = g_AkaoCurTrack;
        {
            u32 temp0;
            u32 temp1;

            temp0 = *(u32 *)(track + 8);
            temp1 = *(u32 *)(track + 0x34);
            temp0 &= mask;
            *(u32 *)(track + 8) = temp0;
            temp0 = *(u32 *)(track + 0xC);
            temp1 &= mask;
            *(u32 *)(track + 0x34) = temp1;
            temp1 = *(u32 *)(track + 0x3C);
            temp0 &= mask;
            *(u32 *)(track + 0xC) = temp0;
            temp0 = *(u32 *)(track + 0x38);
            temp1 &= mask;
            *(u32 *)(track + 0x3C) = temp1;
            temp0 &= mask;
            *(u32 *)(track + 0x38) = temp0;
        }

        if ((*(u32 *)((char *)ptr + 0x38) & 0x800) != 0) {
            *(u32 *)(track + 0x30) &= ~(1 << *(u16 *)((char *)ptr + 0x5C));
        }
    } else {
        SeqOp_DeactivateVoice(ptr);
    }

    *(u32 *)((char *)ptr + 0x38) = 0;
    g_AkaoVoiceUpdateFlags |= 0x10;
    Seq_MarkTrack34MaskDirty();
    Seq_MarkTrack38MaskDirty();
    Seq_MarkTrack3CMaskDirty();
}
