typedef struct Unk8008C70CArg {
    int pad_0;
    int field_4;
} Unk8008C70CArg;

extern char *g_AkaoCurTrack;

void Seq_SetCurrentTrackField56(Unk8008C70CArg *arg0) {
    *(short *)(g_AkaoCurTrack + 0x56) = arg0->field_4;
}
