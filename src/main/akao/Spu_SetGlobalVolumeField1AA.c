
typedef unsigned short u16;

typedef struct AkaoState {
    unsigned char pad[0x1AA];
    u16 field_1AA;
} AkaoState;

extern AkaoState *_spu_RXX;

int Spu_SetGlobalVolumeField1AA(int arg0) {
    register int input asm("$2");
    int value;
    register int bits asm("$2");
    int ret;
    AkaoState *state;
    u16 flags;

    input = arg0;
    value = 0;
    if (input >= 0) {
        value = input;
        if (value >= 0x40) {
            value = 0x3F;
        }
    }

    state = _spu_RXX;
    bits = value & 0x3F;
    bits <<= 8;
    flags = state->field_1AA;
    flags &= 0xC0FF;
    flags |= bits;
    state->field_1AA = flags;
    ret = value;
    return ret;
}
