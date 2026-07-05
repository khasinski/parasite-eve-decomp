/* CC1_PROFILE: build0001 */

extern int g_AkaoPlaybackMode;
extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Seq_MarkDirtyTracks(void *);
void Spu_MarkActiveVoicesDirty(void);

typedef struct {
    int pad_0;
    int field_4;
} AkaoCommand8008C6D0;

extern int g_AkaoVoicePortamentoResetMask;
extern int g_AkaoTrackStateArray[];

void Seq_SetPlaybackMode1AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 1;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack += 0x68;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode4AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 4;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack += 0x68;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetPlaybackMode2AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 2;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack += 0x68;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
    Spu_MarkActiveVoicesDirty();
}

void Seq_SetGlobalD2B8AndDirtyAllTracks(AkaoCommand8008C6D0 *cmd) {
    unsigned int i = 0;
    int value = cmd->field_4;
    int *slot;

    slot = g_AkaoTrackStateArray;
    g_AkaoVoicePortamentoResetMask = value;
    for (; i < 0x18; i++, slot = (int *)((char *)slot + 0x11C)) {
        *slot |= 3;
    }
}
