typedef unsigned int u32;

extern u32 D_8009D2C4;

void Seq_MarkTrack38MaskDirty(void) {
    D_8009D2C4 |= 0x100;
}
