/* PSY-Q LIBSPU S_SNC: SpuSetNoiseClock. */
#include "pe1/psyq_spu_internal.h"

long SpuSetNoiseClock(long clock) {
    int input;
    int value;
    int clockBits;
    SpuRegs *state;
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
    input = value & 0x3F;
    input <<= 8;
    flags = state->spucnt;
    flags &= 0xC0FF;
    clockBits = (u16)input;
    flags |= clockBits;
    /* Ordinary store view preserves the SDK write in the return delay slot. */
    *(u16 *)&state->spucnt = flags;
    return value;
}

unsigned int gap_akao_Spu_SetGlobalVolumeField1AA_tail_7A700[] __attribute__((section(".text"))) = {
    0x00000000,
    0x00000000,
};
