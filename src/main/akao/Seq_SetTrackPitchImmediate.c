/* CC1_PROFILE: build0001 */
typedef unsigned short u16;

extern char *g_AkaoCurTrack;
extern char g_AkaoVoiceStateTable[];
extern char g_AkaoVoiceStateTable2[];

void Seq_MarkDirtyTracks(void *arg0);

void Seq_SetTrackPitchImmediate(int *arg0) {
    int *msg;
    int selector;
    char *track;
    char *primary_track;
    char *next_track;
    register int value asm("$2");
    char *base;

    msg = arg0;
    selector = msg[4];
    if (selector == 0 || selector == *(u16 *)(g_AkaoCurTrack + 0x54)) {
        base = g_AkaoVoiceStateTable;
        value = msg[1];
        primary_track = g_AkaoCurTrack;
        value &= 0x7F;
        value <<= 16;
        *(int *)(primary_track + 0x48) = value;
        *(u16 *)(primary_track + 0x50) = 0;
        Seq_MarkDirtyTracks(base);
    } else if (selector != 0) {
        track = g_AkaoCurTrack;
        if (selector == *(u16 *)(track + 0xBC)) {
            next_track = track + 0x68;
            base = g_AkaoVoiceStateTable2;
            value = msg[1];
            g_AkaoCurTrack = next_track;
            *(u16 *)(track + 0xB8) = 0;
            value &= 0x7F;
            value <<= 16;
            *(int *)(track + 0xB0) = value;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack -= 0x68;
        }
    }
}
