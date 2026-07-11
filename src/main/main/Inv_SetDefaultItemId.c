/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern s32 D_8009D038;
extern u32 D_800A76A4_obj[4] __asm__("D_800A76A4");
extern u8 D_800A1B70[];
extern u8 D_800A1B90[];
extern u8 D_800A1D79[];

#define D_800A76A4 (D_800A76A4_obj[0])

#define SCRAMBLE_RANDOM_BYTES()                    \
    do {                                           \
        int scramble_i_;                           \
        for (scramble_i_ = 0; scramble_i_ < 0x20; scramble_i_++) { \
            D_800A1B90[scramble_i_] ^= D_800A1D79[scramble_i_];    \
        }                                          \
        for (scramble_i_ = 0x20; scramble_i_ < 0x209; scramble_i_++) { \
            D_800A1B90[scramble_i_] ^= D_800A1B70[scramble_i_];    \
        }                                          \
    } while (0)

void Inv_SetDefaultItemId(void) {
    u32 state[0x209];
    u32 seed;
    u32 bits;
    int bit;
    int index;

    seed = D_800A76A4 / 60;
    bits = 0;

    for (index = 0; index < 0x11; index++) {
        for (bit = 0x1F; bit >= 0; bit--) {
            seed = seed * 0x5D588B65 + 1;
            bits = (bits >> 1) | (seed & 0x80000000);
        }
        state[index] = bits;
    }

    state[0x10] = (state[0x10] << 23) ^ (state[0] >> 9) ^ state[0xF];

    for (index = 0x11; index < 0x209; index++) {
        state[index] = (state[index - 0x11] << 23) ^ (state[index - 0x10] >> 9) ^ state[index - 1];
    }

    for (index = 0; index < 0x209; index++) {
        D_800A1B90[index] = state[index];
    }

    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();

    D_8009D038 = 0x208;
}
