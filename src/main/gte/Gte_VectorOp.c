typedef int s32;
typedef short s16;

#include "pe1/gte.h"

extern s16 D_80096250[];

s32 Gte_VectorOp(s32 value, s32 *table_value, s32 *shift_out) {
    s32 leading;
    s32 even_leading;
    s32 shift;
    s32 table_index;

    gte_ldlzcs(value);
    gte_getlzcr(leading);
    if (leading == 0 || leading == 32) {
        return -1;
    }

    even_leading = leading & -2;
    shift = (31 - even_leading) >> 1;
    if (even_leading >= 24) {
        table_index = value << (even_leading - 24);
    } else {
        table_index = value >> (24 - even_leading);
    }
    *shift_out = shift;
    *table_value = D_80096250[table_index - 64];
    return 1;
}
