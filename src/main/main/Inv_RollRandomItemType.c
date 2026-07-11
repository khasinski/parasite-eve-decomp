/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern s32 D_8009D038;
extern u8 D_800A1B70[];
extern u8 D_800A1B90[];
extern u8 D_800A1D79[];
extern u16 D_800A1E4E[];
extern u16 D_800A1E6E[];

#define NEXT_RANDOM_BYTE(out_)                    \
    do {                                          \
        int i;                                    \
        D_8009D038++;                            \
        if (D_8009D038 >= 0x209) {               \
            for (i = 0; i < 0x20; i++) {         \
                D_800A1B90[i] ^= D_800A1D79[i];  \
            }                                     \
            for (i = 0x20; i < 0x209; i++) {     \
                D_800A1B90[i] ^= D_800A1B70[i];  \
            }                                     \
            D_8009D038 = 0;                      \
        }                                         \
        (out_) = D_800A1B90[D_8009D038];         \
    } while (0)

void Inv_RollRandomItemType(s16 *type_out, s16 *amount_out) {
    int type;
    int amount;
    int available_count;
    int random_byte;
    int choices[3];
    u16 *remaining;

    if (type_out == 0 || amount_out == 0) {
        return;
    }

    if (*type_out == 0) {
        available_count = 0;
        for (type = 0; type < 3; type++) {
            if (D_800A1E6E[type * 0x10] != 0) {
                choices[available_count] = type;
                available_count++;
            }
        }

        if (available_count != 0) {
            NEXT_RANDOM_BYTE(random_byte);
            *type_out = ((available_count * random_byte) >> 8) + 1;
        }
    }

    type = *type_out;
    if ((u16)(type - 1) >= 3) {
        return;
    }

    remaining = &D_800A1E4E[type * 0x10];
    amount = *amount_out;

    if (amount == 0) {
        NEXT_RANDOM_BYTE(random_byte);
        amount = (*remaining * random_byte) >> 8;
        *amount_out = amount;
    } else if (amount < 0) {
        amount = (*remaining * -amount) / 100;
        *amount_out = amount;
    }

    if (*remaining < (u16)*amount_out) {
        *amount_out = *remaining;
    }

    *remaining -= *amount_out;
}
