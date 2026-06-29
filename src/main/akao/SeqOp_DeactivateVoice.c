extern int g_SpuActiveVoiceMask;
extern int g_AkaoVoiceMaskScratch;
extern int g_AkaoTrack34Mask;
extern int g_AkaoTrack38Mask;
extern int g_AkaoTrack3CMask;

void SeqOp_DeactivateVoice(char *ptr, int mask) {
    int *first = &g_SpuActiveVoiceMask;

    mask = ~mask;
    *first &= mask;
    g_AkaoTrack34Mask &= mask;
    g_AkaoTrack38Mask &= mask;
    g_AkaoTrack3CMask &= mask;
    g_AkaoVoiceMaskScratch &= mask;
    *(int *)(ptr + 0x2C) = 0;
    *(int *)(ptr + 0x28) = 0;
}
