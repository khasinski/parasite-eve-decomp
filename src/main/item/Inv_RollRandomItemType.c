/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/random.h"

static inline int RandomByte(void) {
    int i;
    if (++D_8009D038 >= 521) {
        for (i = 0; i < 32; ++i)
            D_800A1B90[i] ^= D_800A1B90[i + 489];
        for (i = 32; i < 521; ++i)
            D_800A1B90[i] ^= D_800A1B90[i - 32];
        D_8009D038 = 0;
    }
    return D_800A1B90[D_8009D038];
}
void Inv_RollRandomItemType(s16 *type, s16 *amount) {
    /* Retail builds this list but only uses its count when choosing a type. */
    int available[3];
    int count;
    int i;
    u16 *stock;
    if (type && amount) {
        if (!*type) {
            count = 0;
            for (i = 0; i < 3; ++i)
                if (D_800A1E64[i].ammo) available[count++] = i;
            if (count) *type = ((count * RandomByte()) >> 8) + 1;
        }
        {
            u16 chosen = *type;
            if ((unsigned)(chosen - 1) < 3) {
                stock = &D_800A1E64[(s16)chosen - 1].ammo;
                if (!*amount) *amount = (*stock * RandomByte()) >> 8;
                else if (*amount < 0) *amount = (*stock * -*amount) / 100;
                if (*amount > *stock) *amount = *stock;
                *stock -= *amount;
            }
        }
    }
}
