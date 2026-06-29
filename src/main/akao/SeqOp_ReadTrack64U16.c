/* CC1_FLAGS: -G4 */

extern void *g_AkaoCurTrack;

void SeqOp_ReadTrack64U16(unsigned char **stream) {
    unsigned char *cursor = *stream;
    char *track = g_AkaoCurTrack;

    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) = cursor[0];

    cursor = *stream;
    *stream = cursor + 1;
    *(unsigned short *)(track + 0x64) |= cursor[0] << 8;
}
