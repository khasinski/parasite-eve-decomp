/* CC1_FLAGS: -G4 */

extern void *g_AkaoCurTrack;

void SeqOp_StreamPair(unsigned char **stream) {
    unsigned char *cursor = *stream;
    unsigned char value;
    char *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    *(short *)(track + 0x60) = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    value = cursor[0];
    *(short *)(track + 0x62) = 0;
    *(short *)(track + 0x5E) = 0;
    *(short *)(track + 0x5C) = value;
}

void SeqOp_ReadTrack64U16(unsigned char **stream) {
    unsigned char *cursor = *stream;
    char *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) |= cursor[0] << 8;
}
