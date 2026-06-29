/* CC1_PROFILE: build0001 */
/* MASPSX_FLAGS: --expand-div */
typedef unsigned short u16;

extern char *g_AkaoCurTrack;
extern char g_AkaoVoiceStateTable[];
extern char g_AkaoVoiceStateTable2[];

void Seq_MarkDirtyTracks(void *arg0);

void Seq_TrackPitchSetup(int *arg0) {
    register int raw asm("$2");
    int duration;
    int selector;
    char *track;
    int start;
    register int delta asm("$6");
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    selector = arg0[4];
    if (selector != 0) {
        track = g_AkaoCurTrack;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        start = arg0[2];
        track = g_AkaoCurTrack;
        start &= 0x7F;
        start <<= 16;
        *(int *)(track + 0x48) = start;
        raw = arg0[3];
        raw &= 0x7F;
        delta = raw << 16;
        delta -= start;
        delta = delta / duration;
        base = g_AkaoVoiceStateTable;
        *(u16 *)(track + 0x50) = duration;
        *(int *)(track + 0x4C) = delta;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (selector != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            start = arg0[2];
            start &= 0x7F;
            start <<= 16;
            *(int *)(track + 0xB0) = start;
            raw = arg0[3];
            raw &= 0x7F;
            delta = raw << 16;
            delta -= start;
            delta = delta / duration;
            base = g_AkaoVoiceStateTable2;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = next_track;
            *(int *)(track + 0xB4) = delta;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack -= 0x68;
        }
    }

done:
    return;
}
