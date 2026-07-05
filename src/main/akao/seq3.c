/* MASPSX_FLAGS: --expand-div */

typedef unsigned short u16;

void Seq_StartNestedStreams(void *arg0, void *arg1, void *arg2);

void Spu_ManageVoices(int arg0, int arg1);

typedef unsigned short u16_1;

extern char *g_AkaoCurTrack;
extern char g_AkaoVoiceStateTable[];
extern char g_AkaoVoiceStateTable2[];

void Seq_MarkDirtyTracks(char *arg0);

extern short g_AkaoGlobalPitchSlideCounter;
extern int D_8009D2B4;

void Seq_ApplyGlobalPitch(void);

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

void Seq_SlideTrackPitch(int *arg0) {
    register int raw asm("$2");
    int duration;
    register int target asm("$5");
    register int selector asm("$7");
    char *track;
    char *base;
    char *next_track;

    raw = arg0[1];
    duration = 1;
    if (raw != 0) {
        duration = raw;
    }

    raw = arg0[2];
    selector = arg0[4];
    raw &= 0x7F;
    target = raw << 16;

    if (selector != 0) {
        track = g_AkaoCurTrack;
        if (selector != *(u16 *)(track + 0x54)) {
            goto secondary_track;
        }
    }

    {
        track = g_AkaoCurTrack;
        target = (target - *(int *)(track + 0x48)) / duration;
        base = g_AkaoVoiceStateTable;
        *(u16 *)(track + 0x50) = duration;
        *(int *)(track + 0x4C) = target;
        Seq_MarkDirtyTracks(base);
    }
    goto done;

secondary_track:
    if (arg0 != 0) {
        if (selector == *(u16 *)(track + 0xBC)) {
            target = (target - *(int *)(track + 0xB0)) / duration;
            base = g_AkaoVoiceStateTable2;
            *(u16 *)(track + 0xB8) = duration;
            next_track = track + 0x68;
            g_AkaoCurTrack = next_track;
            *(int *)(track + 0xB4) = target;
            Seq_MarkDirtyTracks(base);
            g_AkaoCurTrack -= 0x68;
        }
    }

done:
    return;
}

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

void Seq_SetGlobalPitchImmediate(void *arg0) {
    int value;

    value = *(u16 *)((char *)arg0 + 4);
    g_AkaoGlobalPitchSlideCounter = 0;
    D_8009D2B4 = value << 16;
    Seq_ApplyGlobalPitch();
}
