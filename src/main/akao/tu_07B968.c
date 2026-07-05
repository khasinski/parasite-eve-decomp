typedef unsigned short u16;

void Seq_StartNestedStreams(void *arg0, void *arg1, void *arg2);

void Spu_ManageVoices(int arg0, int arg1);

/* CC1_PROFILE: build0001 */
typedef unsigned short u16_1;

extern char *g_AkaoCurTrack;
extern char g_AkaoVoiceStateTable[];
extern char g_AkaoVoiceStateTable2[];

void Seq_MarkDirtyTracks(void *arg0);

void Seq_StartRelativeNestedStream(void *arg0) {
    char *base;
    u16 offset;
    void *arg1;
    void *arg2;

    base = *(char **)((char *)arg0 + 4);
    offset = *(u16 *)base;
    if (offset != 0xFFFF) {
        arg1 = (char *)(offset + (int)base) + 4;
    } else {
        arg1 = 0;
    }

    offset = *(u16 *)(base + 2);
    arg2 = 0;
    if (offset != 0xFFFF) {
        arg2 = (char *)(offset + (int)*(char **)((char *)arg0 + 4)) + 4;
    }

    *(void **)((char *)arg0 + 4) = *(void **)((char *)arg0 + 0x14);
    Seq_StartNestedStreams(arg0, arg1, arg2);
}

void Spu_ManageVoicesCmd(int *arg0) {
    Spu_ManageVoices(arg0[1], arg0[2]);
}

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
    if (selector == 0 || selector == *(u16_1 *)(g_AkaoCurTrack + 0x54)) {
        base = g_AkaoVoiceStateTable;
        value = msg[1];
        primary_track = g_AkaoCurTrack;
        value &= 0x7F;
        value <<= 16;
        *(int *)(primary_track + 0x48) = value;
        *(u16_1 *)(primary_track + 0x50) = 0;
        Seq_MarkDirtyTracks(base);
    } else if (selector != 0) {
        track = g_AkaoCurTrack;
        if (selector == *(u16_1 *)(track + 0xBC)) {
            next_track = track + 0x68;
            base = g_AkaoVoiceStateTable2;
            value = msg[1];
            g_AkaoCurTrack = next_track;
            *(u16_1 *)(track + 0xB8) = 0;
            value &= 0x7F;
            value <<= 16;
            *(int *)(track + 0xB0) = value;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack -= 0x68;
        }
    }
}
