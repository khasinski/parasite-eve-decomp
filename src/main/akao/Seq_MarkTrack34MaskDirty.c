extern unsigned int g_AkaoVoiceUpdateFlags;

void Seq_MarkTrack34MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}
