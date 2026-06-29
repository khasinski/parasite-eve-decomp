extern unsigned int g_AkaoVoiceUpdateFlags;
extern char *g_AkaoCurTrack;

void SeqOp_SetTempoSlide(void *ptr)
{
    unsigned char *stream = *(unsigned char **)ptr;
    char *track;
    unsigned int value;

    *(unsigned char **)ptr = stream + 1;
    value = *stream << 16;
    *(unsigned char **)ptr = stream + 2;
    track = g_AkaoCurTrack;
    value |= stream[1] << 24;
    *(short *)(track + 0x58) = 0;
    g_AkaoVoiceUpdateFlags |= 0x80;
    *(unsigned int *)(track + 0x40) = value;
}
