typedef unsigned int u32;

extern u32 D_8009D2C4;

void Seq_MarkTrack34MaskDirty(void) {
    D_8009D2C4 |= 0x100;
}
