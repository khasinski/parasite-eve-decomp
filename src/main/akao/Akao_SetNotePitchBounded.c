typedef short s16;

typedef struct {
    char pad[0xE2];
    s16 fieldE2;
} AkaoTrackLike;

void Akao_SetVoiceLoopAddr(AkaoTrackLike *track, int arg1);
void Akao_SetVoiceLoopAddrAlt(AkaoTrackLike *track, int arg1);

void Akao_SetNotePitchBounded(AkaoTrackLike *track, int arg1) {
    int value = track->fieldE2;

    if ((unsigned int)value < (unsigned int)arg1) {
        Akao_SetVoiceLoopAddr(track, arg1);
    } else if ((unsigned int)arg1 < (unsigned int)value) {
        Akao_SetVoiceLoopAddrAlt(track, arg1);
    }
}
