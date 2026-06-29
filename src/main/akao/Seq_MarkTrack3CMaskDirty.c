extern unsigned int g_AkaoVoiceUpdateFlags;

void Seq_MarkTrack3CMaskDirty(void) {
    g_AkaoVoiceUpdateFlags |= 0x100;
}
