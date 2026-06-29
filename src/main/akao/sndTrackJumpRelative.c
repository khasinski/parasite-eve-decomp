#define READ_S16(addr) ((short)(*(addr)++ | (*(addr)++ << 8)))

void sndTrackJumpRelative(unsigned char **stream) {
    *stream += READ_S16(*stream);
}
