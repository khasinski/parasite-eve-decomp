/* CC1_PROFILE: build0001 */

extern int g_AkaoPlaybackMode;
extern char g_AkaoVoiceStateTable[];
extern char *g_AkaoCurTrack;

void Seq_MarkDirtyTracks(void *);
void Spu_MarkActiveVoicesDirty(void);

void Seq_SetPlaybackMode2AndRefreshVoices(void) {
    char *base = g_AkaoVoiceStateTable;

    g_AkaoPlaybackMode = 2;
    Seq_MarkDirtyTracks(base);
    g_AkaoCurTrack += 0x68;
    Seq_MarkDirtyTracks(base + 0x1AA0);
    g_AkaoCurTrack -= 0x68;
    Spu_MarkActiveVoicesDirty();
}
