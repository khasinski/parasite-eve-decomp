#include "common.h"
typedef struct SpuNoiseControlRegs {
    unsigned char pad[0x1AA];
    u16 spucnt;
} SpuNoiseControlRegs;

extern SpuNoiseControlRegs *_spu_RXX;

long SpuSetNoiseClock(long clock) {
    register int input asm("$2");
    int value;
    register int bits asm("$2");
    int ret;
    SpuNoiseControlRegs *state;
    u16 flags;

    input = clock;
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
    flags = state->spucnt;
    flags &= 0xC0FF;
    flags |= bits;
    state->spucnt = flags;
    ret = value;
    return ret;
}
