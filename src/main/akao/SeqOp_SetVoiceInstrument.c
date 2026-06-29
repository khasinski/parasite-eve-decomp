typedef unsigned char u8;
typedef unsigned int u32;

void SeqOp_SetVoiceInstrument(void *track_arg, void *src_arg, int arg2) {
    u8 *track = track_arg;
    u8 *src = src_arg;
    int flags;
    int value;
    u32 flags2;
    int tail_value;

    *(int *)(track + 0xF8) = arg2;
    *(int *)(track + 0xFC) = *(int *)(src + 4);
    *(short *)(track + 0x10E) = src[8];
    *(short *)(track + 0x110) = src[9];
    *(short *)(track + 0x112) = src[10];
    *(short *)(track + 0x114) = src[11];
    *(int *)(track + 0x100) = src[13];

    flags = *(int *)(track + 0x38);
    value = src[14];
    *(int *)(track + 0x104) = value;

    if ((flags & 0x200) != 0) {
        *(u32 *)(track + 0xF4) |= 0x1BB80;
    } else {
        unsigned int v = src[12];
        short *dst = (short *)(track + 0x116);

        *dst = v;
        flags2 = *(u32 *)(track + 0xF4);
        tail_value = src[15];
        *(u32 *)(track + 0xF4) = flags2 | 0x1FF80;
        *(int *)(track + 0x108) = tail_value;
    }
}
