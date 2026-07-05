typedef short s16;

typedef struct {
    char pad[0xE2];
    s16 fieldE2;
} AkaoTrackLike;

void Akao_SetVoiceLoopAddr(AkaoTrackLike *track, int arg1);
void Akao_SetVoiceLoopAddrAlt(AkaoTrackLike *track, int arg1);

typedef unsigned char u8;
typedef unsigned short u16;

extern int g_AkaoPitchPeriodTable[];

void Akao_SetNotePitchBounded(AkaoTrackLike *track, int arg1) {
    int value = track->fieldE2;

    if ((unsigned int)value < (unsigned int)arg1) {
        Akao_SetVoiceLoopAddr(track, arg1);
    } else if ((unsigned int)arg1 < (unsigned int)value) {
        Akao_SetVoiceLoopAddrAlt(track, arg1);
    }
}

int Akao_LookupPitchPeriod(int arg0, int arg1, int arg2) {
    register int offset asm("$2");
    int row;
    register int shift asm("$5");
    int shamt;
    int value;
    register int table asm("$3");
    int *base;
    int row_offset;

    arg1 = (u8)arg1;
    row_offset = arg0 << 6;
    table = (int)g_AkaoPitchPeriodTable;
    base = (int *)(row_offset + table);
    row = (unsigned int)arg1 / 12;
    offset = arg1 - (row * 12);
    offset <<= 2;
    value = *(int *)(offset + (int)base);
    shift = row;

    if (arg2 != 0) {
        value += (unsigned int)(value * arg2) >> 7;
    }

    if ((unsigned int)shift < 7) {
        goto less_than_7;
    }
    shamt = shift - 6;
    value <<= shamt;
    goto done;

less_than_7:
    if ((unsigned int)row >= 6) {
        goto done;
    }
    shamt = 6 - row;
    value = (unsigned int)value >> shamt;

done:
    return value & 0xFFFF;
}
