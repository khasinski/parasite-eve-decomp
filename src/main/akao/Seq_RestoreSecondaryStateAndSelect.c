/* CC1_PROFILE: build0001 */
typedef unsigned int u32;

extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Util_CopyWords(void *dst, void *src, int len);
void Akao_StepSequencerVoice(int arg0);

void Seq_RestoreSecondaryStateAndSelect(int *arg0) {
    char *state;

    state = g_AkaoCurTrack;
    if ((*(u32 *)(state + 4) != 0) && (*(u32 *)(state + 0x6C) == 0)) {
        Util_CopyWords(state, state + 0x68, 0x68);
        Util_CopyWords(g_AkaoVoiceStateTable, g_AkaoVoiceStateTable + 0x1AA0, 0x1AA0);
    }

    Akao_StepSequencerVoice(arg0[1]);
    *(unsigned short *)(g_AkaoCurTrack + 0x54) = arg0[3];
}
