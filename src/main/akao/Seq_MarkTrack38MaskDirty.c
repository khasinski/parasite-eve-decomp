extern unsigned int g_AkaoVoiceUpdateFlags;

void Seq_MarkTrack38MaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}
